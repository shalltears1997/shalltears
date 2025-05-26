#include <iostream>
#include <cmath>
#include <chrono>
#include <thread>
#include <fstream>
#include <vector>

#include "gv_extern.hpp"
#include "l_tk-library.hpp"


PathGenerator::PathGenerator(PotentialFunction ptf, std::vector<DATA_grid_part> low_grids_in, DATA_xy robot)
{
	p_ptf = ptf;
	p_robot = robot;
	low_grids = low_grids_in;
}


bool PathGenerator::valley_judge(double x, double y)
{
	// ロボット位置が谷かどうか判定（谷ならtrue）
	bool valley_flag_x = (p_ptf.potential(x-PATH_VALLEY_RESO,y) >= p_ptf.potential(x,y)) && p_ptf.potential(x+PATH_VALLEY_RESO,y) >= p_ptf.potential(x,y);
	bool valley_flag_y = ( (p_ptf.potential(x,y-PATH_VALLEY_RESO) >= p_ptf.potential(x,y)) && (p_ptf.potential(x,y+PATH_VALLEY_RESO) >= p_ptf.potential(x,y)) );

	return (valley_flag_x && valley_flag_y);
}

bool PathGenerator::new_method(double x, double y, DATA_xy *value)
{
	bool flag;
	double dx = 0, dy = 0;

	// ロボット位置が谷かどうか判定（谷ならtrue）
	bool valley_flag_x = (p_ptf.potential(x-PATH_VALLEY_RESO,y) >= p_ptf.potential(x,y)) && (p_ptf.potential(x+PATH_VALLEY_RESO,y) >= p_ptf.potential(x,y));
	bool valley_flag_y = (p_ptf.potential(x,y-PATH_VALLEY_RESO) >= p_ptf.potential(x,y)) && (p_ptf.potential(x,y+PATH_VALLEY_RESO) >= p_ptf.potential(x,y));

	// x方向もy方向も谷の場合
	if(valley_flag_x && valley_flag_y)
	{
		// その場で停止
		dx = 0;
		dy = 0;

		flag = false;
	}
	else
	{
		// 谷の場合はその方向は停止、谷でなければ高さの差分を算出
		if(valley_flag_x) dx = 0;
		else dx = p_ptf.potential(x-PATH_NEXT_RESO, y) - p_ptf.potential(x+PATH_NEXT_RESO, y);
		if(valley_flag_y) dy = 0;
		else dy = p_ptf.potential(x, y-PATH_NEXT_RESO) - p_ptf.potential(x, y+PATH_NEXT_RESO);

		// 高さの差分が無限大の場合は正規化
		//（xとyともに無限大なら両方1に、片方のみ無限大の場合はそれを1にしてもう片方は0に）
		if(std::isinf(dx) || std::isinf(dy))
		{
			if(!std::isinf(dx)) dx = 0;
			else if(dx>0) dx=1;
			else dx=-1;

			if(!std::isinf(dy)) dy = 0;
			else if(dy>0) dy=1;
			else dy=-1;
		}

		// 正規化
		double power_of_pfield = calDistance(dx, dy);
		dx *= PATH_NEXT_RESO/power_of_pfield;
		dy *= PATH_NEXT_RESO/power_of_pfield;

		(*value).x = dx;
		(*value).y = dy;

		flag = true;
	}

	return flag;
}


bool PathGenerator::goal_reach_judge(DATA_xy point)
{
	for(int k=0; k<low_grids.size(); k++)
	{
		if(calDistance( grid2double(low_grids[k].grid), point ) <= (double)(WIDTH_ETHO/gridnum1)) return true;
	}
	return false;
}



// 入力されたゴールの位置と、出力する経路の位置に対して適用する
void PathGenerator::potential_barrier()
{
	// ロボットがゴールに到達したとき、バリアフラグを解除する
	if( goal_reach_judge({p_robot.x, p_robot.y}) )
	{
		barrier_removeAll_flag = true;
		// return true;
		// 判定はするが、以下の計算はとりやめない（return trueはコメントアウトにしておく）
		// なぜなら、計算を続けていればもっと近くまで移動できるから
	}
	// 行き止まりにはまった：ゴールは到達していないが、最低点にはまってしまった
	// ここに負のポテンシャルを張るためのフラグを立てる
	//else if(valley_judge(p_robot.x, p_robot.y))
	// シミュレータで経路を一つずつ辿る場合は上の条件で酔いが、実用的ではない
	// 実機では3グリッドほど（もしくはそれに相当する経路3つ分）ずれることが想定される
	else
	{
		int judge = 0;
		judge += valley_judge(p_robot.x, p_robot.y);
		// for(int i=0; i<1; i++) judge += valley_judge(p_route_point[i].x, p_route_point[i].y);
		if(judge > 0)
		{
			DATA_xy direction;
			new_method(p_robot.x, p_robot.y, &direction);

			// ロボットから目的地をみたときの後方にバリアを設定
			// if(dirextion.x > 0) barrier_x = p_robot.x - 200;
			// else barrier_x = p_robot.x + 200;
			// if(dirextion.y > 0) barrier_y = p_robot.y - 200;
			// else barrier_y = p_robot.y + 200;

			// 壁側にバリアを設定
			if(p_robot.x >    0) barrier_x = p_robot.x + 100;
			else barrier_x = p_robot.x - 100;
			if(p_robot.y > 1500) barrier_y = p_robot.y + 100;
			else barrier_y = p_robot.y - 100;

			barrier_set_flag = true;
			printf("a barrier is being generated at: %7.1lf  %7.1lf\n", p_robot.x, p_robot.y);
		}
	}
}

// 経路を引く（要素0はロボットの現在位置）
std::vector<DATA_xy> PathGenerator::generate_path()
{
	// if(!goal_reach_judge({p_robot.x, p_robot.y}))
	// {
	// 	potential_barrier();
	// }

	// 先頭にロボットの位置を設定
	p_route_point.clear();
	p_route_point.push_back(p_robot);

	for(int tki=0; tki+1<tki_MAX; tki++)
	{
		// p_tki_length = tki;	// もうちょっとうまい決め方があるかな

		// 経路が最低地点まで延びたので、経路の計算は停止（ロボットが本当のゴールに到達したとは限らない）
		if(valley_judge(p_route_point[tki].x, p_route_point[tki].y))
		{
			break;
		}

		// 新手法：偏微分から方向を求める
		DATA_xy delta;
		new_method(p_route_point[tki].x, p_route_point[tki].y, &delta);

		// 経路の次の点を決定
		p_route_point.push_back({p_route_point[tki].x + delta.x, p_route_point[tki].y + delta.y});

		if(std::isnan(p_route_point[tki].x + delta.x) || std::isnan(p_route_point[tki].y + delta.y))
		{
			std::this_thread::sleep_for(std::chrono::seconds(10));
			printf("in tk44 h.76: %7.3lf  %7.3lf\n", p_route_point[tki].x + delta.x, p_route_point[tki].y + delta.y);
		}

		if(tki==0)
		{
			// printf("%7.3lf  %7.3lf  %7.3lf\n",
			// 	p_ptf.potential(p_route_point[tki].x-PATH_VALLEY_RESO, p_route_point[tki].y+PATH_VALLEY_RESO), p_ptf.potential(p_route_point[tki].x, p_route_point[tki].y+PATH_VALLEY_RESO), p_ptf.potential(p_route_point[tki].x+PATH_VALLEY_RESO, p_route_point[tki].y+PATH_VALLEY_RESO));
			// printf("%7.3lf  %7.3lf  %7.3lf\n",
			// 	p_ptf.potential(p_route_point[tki].x-PATH_VALLEY_RESO, p_route_point[tki].y), p_ptf.potential(p_route_point[tki].x, p_route_point[tki].y), p_ptf.potential(p_route_point[tki].x+PATH_VALLEY_RESO, p_route_point[tki].y));
			// printf("%7.3lf  %7.3lf  %7.3lf\n",
			// 	p_ptf.potential(p_route_point[tki].x-PATH_VALLEY_RESO, p_route_point[tki].y-PATH_VALLEY_RESO), p_ptf.potential(p_route_point[tki].x, p_route_point[tki].y-PATH_VALLEY_RESO), p_ptf.potential(p_route_point[tki].x+PATH_VALLEY_RESO, p_route_point[tki].y-PATH_VALLEY_RESO));
			// printf("%7.3lf  %7.3lf | %7.3lf  %7.3lf\n\n",
			// 	p_route_point[tki].x, p_route_point[tki].y,
			// 	p_route_point[tki+1].x, p_route_point[tki+1].y);
		}
		// 領域からはみ出ないようにする（もうこれはなくても大丈夫、塚田2017/11/11）
		// p_route_point[tki+1] = adjust_point(p_route_point[tki+1]);
	}
	return p_route_point;
}
