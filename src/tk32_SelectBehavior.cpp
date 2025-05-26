#include <iostream>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <algorithm>  // vectorのソート

#include "class.hpp"
#include "gv_extern.hpp"
#include "tk30_ActivateBehavior.hpp"
#include "l_tk-library.hpp"

#include "tk22_SelectBehavior.hpp"

#include "tk_Parameter.hpp"


// 行動選択
int BehaviorSelectorClass::select(std::vector<DATA_human> human, PARAMETER pa_human[], BALL_yoheisan ball_yo, BALL ball_st, bool sound_move, ConfigDataClass config_data_in, NAZO nazo)
{


	//////////////////////////////////////////////////////////////////////////////
	// モード選択
	//////////////////////////////////////////////////////////////////////////////

	int mode = 0;
	int i,j;
	DATA_xy position={};
	position = (data.object[data.object_maxID].posi);

	// 各モードの、それ自体の「必要度」算出
	std::vector<double> behavior_itsown_necessity(8);

	// モード0：リアクションモード（生存本能）：ボール、音への反応
	// -> 外界の刺激＝外界由来のストレスに適切に反応する。
	// -> たとえば音に対する反応の必要度は0.7、ボールは0.5をとするなどして、必要性を区別できる。
	// -> ただし、音が鳴ったことやボールが見えたことを示すためにも、頭部動作にて「反射」は示す。とはいえ反射は持続的ではないため、モードの括りではない。

	{
		double ball = 0;
		if(data_pre.motion_id == PLAY_USING_BALL)
		{
			ball = data.ball_play_required_level;
		}
		else
		{
			ball = data.ball_st.ball_presence * 100;
		}

		double sound = 0;

		double shagamu = (human[0].height < data.shagamu_th && exist_judge_human(human[0].posi)) * 100;

		std::vector<double> level = {ball, sound, shagamu};

		double max = *std::max_element(level.begin(), level.end());
		behavior_itsown_necessity[0] = max/100;
	}



	// ↑ この値を、受け取った刺激によって変化させる

	// モード1：タスクモード（義務履行）：監視行動
	// -> 与えられたタスクを実行する。

	if(data.object_maxID != -1)behavior_itsown_necessity[1] = data.object[data.object_maxID].monitoring_necessity;
	else if(data.object_maxID ==-2)behavior_itsown_necessity[1] = data.human[0].monitoring_necessity;
	else behavior_itsown_necessity[1] = data.monitoring_necessity[data.max_area_grid.i][data.max_area_grid.j];

	//behavior_itsown_necessity[1] = 0;

	// モード2：ストレスモード（関係構築）：愛着行動
	// -> 人との関係性に応じた社会的なふるまい。ストレスを減らすのに有利な行動をとることで次第に達成される。
	//
	// ストレスパラメータ(実効値) -> ストレス実行パラメータ(理論値)
	// 現在の人の有無を参照し、減らせるパラメータかどうかを考慮する
	// // double Miss_a = data.pa_human[0].miss.value * exist_judge_human(data.human[0].posi);
	// double Miss_a = data.pa_human[0].mis_o * exp(-data.human_non_exist_duration[0]/(10+data.sensitivity_of_anxiety/2));
	// double Anxiety_a = data.pa_human[1].anx_o * exist_judge_human(data.human[0].posi) * exist_judge_human(data.human[1].posi);
	// double Explore_a = data.pa_human[1].mis_o * exist_judge_human(data.human[1].posi);
	double Miss_a = data.pa_human[0].mis;// * exp(-data.human_non_exist_duration[0]/(data.TimeConstant1*data.K_mis));;
	double Anxiety_a = data.pa_human[1].anx;// * exp(-data.human_non_exist_duration[0]/(data.TimeConstant1*data.K_anx))
																							 // * exp(-data.human_non_exist_duration[1]/(data.TimeConstant2*data.K_anx));;
	double Explore_a = data.pa_human[1].exp;// * exp(-data.human_non_exist_duration[1]/(data.TimeConstant2*data.K_exp));
	//
	std::vector<double> stress = {Miss_a, Anxiety_a, Explore_a, data.explore};
	double max = *std::max_element(stress.begin(), stress.end());
	// behavior_itsown_necessity[2] = (double)(100 - max)/100;
	behavior_itsown_necessity[2] = max/100;
	// PRINT("ストレスモードの必要度: %lf", behavior_itsown_necessity[2]);

	// モード3：リラックスモード（娯楽）：能動的なボール遊び、GoToストレンジャー
	behavior_itsown_necessity[3] = 0;//data.sensitivity_of_anxiety/100;

	// モード自体の「必要度」デバッグ
	// for(int i=0; i<8; i++)PRINT("それ自体の必要性(#%d): %lf", i, behavior_itsown_necessity[i]);

	//////////////////////////////////////////////////////////////////////////////

	// モード選択 実施
	std::vector<double> necessity_ratio(8);
	std::vector<double> necessity_value(8);

	necessity_ratio[0] = 1 * (1 - 0);
	necessity_value[0] = necessity_ratio[0] * behavior_itsown_necessity[0];
	// PRINT("必要性(#%d): %lf", 0, necessity_value[0]);

	for(int i=1; i<8; i++)
	{
		// 「今のモードに与えられる必要性の最大値の枠」＝「前のモードの必要性」×「前のモードの必要無さによって生じる今の振る舞いの必要性」
		necessity_ratio[i] = necessity_ratio[i-1] * (1 - behavior_itsown_necessity[i-1]);
		// 「今のモードの必要性」＝「今のモードに与えられる必要性の最大値の枠」×「今のモード自体の必要性」
		necessity_value[i] = necessity_ratio[i] * behavior_itsown_necessity[i];
		// PRINT("必要性(#%d): %lf", i, necessity_value[i]);
	}

	for(int i=1; i<8; i++) necessity_value[i] -= 0.1 * (data_pre.mode > i);


	//////////////////////////////////////////////////////////////////////////////

	// std::vector<double> stress = {Miss_a, Anxiety_a, Explore_a, data.explore};
	std::vector<double>::iterator necessity_max = std::max_element(necessity_value.begin(), necessity_value.end());
	mode = std::distance(necessity_value.begin(), necessity_max);
	// mode = 2;
	//data.mode = mode;
	// PRINT("実行するモード: %d", mode);

	/*
	if(behavior_itsown_necessity[1]>0.7 && data.object_maxID !=-1)
	{
		mode = 1;
	}else if(behavior_itsown_necessity[1]>0.8 && data.object_maxID == -1)
	{
		mode = 1;
	}else mode = 2;*/

	data.mode = mode;

	// PRINT("実行するモード: %d", mode);

	if(data.motion_id == MONITORING_AREA && data.monitoring_necessity[data.max_area_grid.i][data.max_area_grid.j] > 0.1)
	{
		mode = 1;
	}

	print_delta = data.monitoring_necessity[data.max_area_grid.i][data.max_area_grid.j];

	//////////////////////////////////////////////////////////////////////////////
	// 行動選択
	//////////////////////////////////////////////////////////////////////////////

	int behavior_id;

	//////////////////////////////////////////////////////////////////////////////
	// リアクションにおける行動選択：音への反応、ボール遊び
	//////////////////////////////////////////////////////////////////////////////

	if(mode==0)
	{

		// オーナーがしゃがんだとき
		if(human[0].height < data.shagamu_th && exist_judge_human(human[0].posi))
		{
			behavior_id = GO_TO_SITTING_OWNER;
		}
		// ボール遊び
		//else
		{//暴発したので仮に
			//behavior_id = GO_TO_SITTING_OWNER;
//behavior_id = PLAY_USING_BALL;
		}

		/*
		// 音
		if (sound_move)
		{
			return SOUND_MOVE;
		}
		// ボール遊び
		else if( ball_yo.t_id!=0 && ball_st.ball_time_interval>ball_st.ball_time_interval_th && ball_st.ball_priority )
		{
			//select_motion = PLAY_USING_BALL;
			return PLAY_USING_BALL;
		}
		// 通常時のボール遊び選択：ボールがあるならボール遊び
		else if( ball_yo.t_id!=0 && ball_st.ball_time_interval>ball_st.ball_time_interval_th && pa_human[0].ata >= 49 )
		{
			//select_motion = PLAY_USING_BALL;
			return PLAY_USING_BALL;
		}
		// 新たに追加した音源定位、ボール遊び（優先選択版）が選択された場合はここでこの関数を抜ける
		if (select_motion != -1) return select_motion;
		*/


	}

	//////////////////////////////////////////////////////////////////////////////
	//タスクモードにおける目的地選択（通知行動）
	//////////////////////////////////////////////////////////////////////////////
/*	int owner=0;

	for(int i=1;i<6;i++)
	{
		if(C_object[i].getownerflag())
		{
			owner++;
		}
	}

	if(mode==1)
	{
		if(data.object_maxID != -1)
		{
			if(exist_judge_human(data.human[0].posi) == false && exist_judge_human(data.human[1].posi) == false)
			{
				behavior_id = ROTATE;
			}
			/*else if(owner > 0)
			{
				behavior_id = MONITORING_USER;
			}*//*
			else if(C_object[data.object_maxID].getblind())
			{
				behavior_id = NOTIFY;
			}
			else
			{
				behavior_id = MONITORING_OBJECT;
			}
		}
		else behavior_id = MONITORING_AREA;
	}*/

	if(mode==1)
	{
		if(data.object_maxID != -1)
		{
			/*if(exist_judge_human(data.human[0].posi) == false && exist_judge_human(data.human[1].posi) == false)
			{
				behavior_id = ROTATE;
			}
			else if(data.object_maxID==-2)
			{
				behavior_id = MONITORING_USER;
			}
			else if(C_object[data.object_maxID].getblind())
			{
				behavior_id = NOTIFY;
			}
			else
			{
				behavior_id = MONITORING_OBJECT;
			}*/

			if(calDistance(data.robot.posi.x-position.x,data.robot.posi.y-position.y)<900 && calDistance(data.human[0].posi.x-position.x,data.human[0].posi.y-position.y)<1000)
			{
				behavior_id = GAZE_ALTERNATION;
			}
			else if(calDistance(data.robot.posi.x-data.human[0].posi.x,data.robot.posi.y-data.human[0].posi.y)<800  || data.flag_direction==true)
			{
				behavior_id = SHOWING_DIRECTION;
			}
			else
			{
				behavior_id = ATTENTION_GETTING;
			}

		}
		else behavior_id = MONITORING_AREA;
	}

	//////////////////////////////////////////////////////////////////////////////
	// ストレスモードにおける目的地選択（愛着行動）
	//////////////////////////////////////////////////////////////////////////////

	if(mode==2)
	{
		// 一番上の方で算出したストレス実行パラメータが高ければ、それを減らすふるまいを選択する、というのが基本発想
		// ただし、現在最大のストレスを減らすために移動すると、別のストレスが上がり、今度はそのストレスを解消するために移動するなど、無駄が生じる
		// --> それが愛着行動の特別な点。ストレス変化は移動すれば付いて回るもの。目の前のストレスを行動要因と称してしまっては、賢いとはいえない
		// そこで、逆転の発想で、あらかじめ考え得る目的地に向かった場合にどれほどのストレスを低減できるかを算出し、最も効果的な目的地を選ぶ
		// 考慮するもの：目的地までの距離、人の有無、人の位置、行動特性パラメータ	/ これにより、無駄にパタパタしなくなる。といいなと思ってる

		// 目的地決定
		calcParmClass parm(data);
		parm.set_tmp_parm(Miss_a, Anxiety_a, Explore_a);  // 仮処置
		parm.dExplorePlace2_surplus_sum(&data.unknown_area_finer);
		data.flag_direction = false;
		data.flag_attention = false;


		// 全グリッドに関して、そこに移動した場合のパラメータ変化量の予測値を算出する
		// パラメータの減少幅が最大（マイナスの値が大きくなる）場所を目的地とする
		DATA_grid_part min = {{double2grid_x(data.robot.posi.x),double2grid_y(data.robot.posi.y)}, std::numeric_limits<double>::infinity()};
		// DATA_grid_part min = {{double2grid_x(data.robot.posi.x),double2grid_y(data.robot.posi.y)}, parm.expectedDelta("try", data.robot.posi, &data.unknown_area_finer)};

		//ボーカリゼーションのための現環境内におけるストレスポテンシャル最大の場所（最も減少効率が悪いロボットが不安に感じるエリア）を記録するための変数
		//DATA_grid_part st_max = {{double2grid_x(data.robot.posi.x),double2grid_y(data.robot.posi.y)}, std::numeric_limits<double>::infinity()};
		//DATA_grid_part st_min = {{double2grid_x(data.robot.posi.x),double2grid_y(data.robot.posi.y)}, std::numeric_limits<double>::infinity()};

		for(int i=0; i<gridnum1; i++)
		{
			for(int j=0; j<gridnum2; j++)
			{
				DATA_xy goal = {grid2double_x(i), grid2double_y(j)};

				// data.stress_parm_grid[i][j] = parm.expectedDelta_score("try", goal, &data.unknown_area_finer) + 0.05 * calDistance(goal, data.robot.posi)/1000 * 10;

								PositionClass own, str;
								own.setPos(data.human[0].posi);
								str.setPos(data.human[1].posi);


				// 予測観測差分値（距離と信頼度によって決まる）
				// owner
				double next_dmisP = parm.dMissP(goal, own, data.pa_human[0].ata, true)
																	+ parm.dMiss_Door(goal, own);
				double next_dmisM = parm.dMissM(goal, own, data.pa_human[0].ata, true);
				// stranger
				double next_danxP = parm.dAnxietyP(goal, str, data.pa_human[1].ata)
																	+  (1 / attachment_filter(data.pa_human[1].ata)) * parm.dAnxiety_Coming(data.robot.posi, str);

				double next_danxM = parm.dAnxietyM(goal, own, data.pa_human[0].ata);

				double next_dexpP = parm.dExploreP(goal, str, data.pa_human[1].ata);
				double next_dexpM = parm.dExploreM(goal, str, data.pa_human[1].ata);

				//print_delta  =  (2 / attachment_filter(data.pa_human[1].ata)) * parm.dAnxiety_Coming(data.robot.posi, str);

				//data.dAnxiety_move[i][j] = data.pa_human[1].anx * (2 / attachment_filter(data.pa_human[1].ata)) * parm.dAnxiety_Coming(data.robot.posi, str);
// PRINT("%lf  %lf", next_dmisP, next_dmisM);
				// 予測認知差分値（感受性と、最小値・最大値が考慮される）
				// double next_cog_dmis = parm.adjust_delta(data.pa_human[0].mis, data.K_mis * next_dmisP + next_dmisM);
				// double next_cog_danx = parm.adjust_delta(data.pa_human[1].anx, data.K_anx * next_danxP + next_danxM);
				// double next_cog_dexp = parm.adjust_delta(data.pa_human[1].exp, data.K_exp * next_dexpP + next_dexpM);
				// double next_cog_dmis = data.K_mis * next_dmisP + next_dmisM;
				// double next_cog_danx = data.K_anx * next_danxP + next_danxM;
				// double next_cog_dexp = data.K_exp * next_dexpP + next_dexpM;

/*
				double next_cog_dmis = next_dmisP + next_dmisM;
				double next_cog_danx = next_danxP + next_danxM;
				double next_cog_dexp = next_dexpP + next_dexpM;


				// 認知移動得点
				double next_eff_mis = data.pa_human[0].mis * next_cog_dmis;
				double next_eff_anx = data.pa_human[1].anx * next_cog_danx;
				double next_eff_exp = data.pa_human[1].exp * next_cog_dexp;

				data.dMiss_score[i][j] = next_eff_mis;
				data.dAnxiety_score[i][j] = next_eff_anx;
				data.dExploreHuman_score[i][j] = next_eff_exp;
*/


				data.dMiss_score[i][j] = data.pa_human[0].mis * (next_dmisP + next_dmisM);
				data.dAnxiety_score[i][j] = data.pa_human[1].anx * (next_danxP + attachment_filter(data.pa_human[0].ata) * next_danxM);

				//data.dAnxiety_score_fv[i][j] = data.pa_human[1].anx * (next_danxP + attachment_filter(data.pa_human[0].ata) * next_danxM);
				//data.dExploreHuman_score[i][j] = data.pa_human[1].exp * (next_dexpP + next_dexpM);

				data.dExploreHuman_score[i][j] = data.pa_human[1].exp * (next_dexpP + next_dexpM);

				// 実行移動得点（離別時間が考慮される）
				// data.dMiss_score[i][j] = next_eff_mis * exp(-data.human_non_exist_duration[0]/(data.TimeConstant1*data.K_mis));
				// data.dAnxiety_score[i][j] = next_eff_anx * exp(-data.human_non_exist_duration[0]/(data.TimeConstant1*data.K_anx))
				// 																				 * exp(-data.human_non_exist_duration[1]/(data.TimeConstant2*data.K_anx));
				// data.dExploreHuman_score[i][j] = next_eff_exp * exp(-data.human_non_exist_duration[1]/(data.TimeConstant2*data.K_exp));

				// 昨年のもの
				// data.dMiss_score[i][j] = Miss_a * parm.adjust_delta(data.pa_human[0].mis_o, parm.dMiss(goal, true));
				// data.dAnxiety_score[i][j] = Anxiety_a * parm.adjust_delta(data.pa_human[1].anx_o, parm.dAnxiety(goal));
				// data.dExploreHuman_score[i][j] = Explore_a * parm.adjust_delta(data.pa_human[1].exp_o, parm.dExploreHuman(goal));

				data.dExplorePlace_score[i][j] = data.explore * parm.adjust_delta(data.explore, parm.dExplorePlace2(goal, &data.unknown_area_finer));

				//なつき度フィルタをかけるところ（missにかけてるのはなつき度の高まりをより強調させることができると考えたため）
				data.dSum_score[i][j] = attachment_filter(data.pa_human[0].ata) * data.dMiss_score[i][j] + data.dAnxiety_score[i][j] + data.dExploreHuman_score[i][j] + data.dExplorePlace_score[i][j];
				// data.stress_parm_grid[i][j] = data.dSum_score[i][j] + 0.05 * calDistance(goal, data.robot.posi)/1000 * 10;
				// data.stress_parm_grid[i][j] = data.dSum_score[i][j] + calDistance(goal, data.robot.posi)/1000 * 15;
				//data.stress_parm_grid[i][j] = data.dSum_score[i][j] + calDistance(goal, data.robot.posi)/1000 * 15;

				//ここがストレスポテンシャルの最終値を計算するところ
				data.stress_parm_grid[i][j] = data.dSum_score[i][j] + calDistance(goal, data.robot.posi)/1000 * 15;

				//data.stress_parm_grid[i][j] = attachment_filter(data.pa_human[0].ata)*(data.dSum_score[i][j] + calDistance(goal, data.robot.posi)/1000 * 15);

				// if(abs(i-double2grid_x(data.robot.posi.x))>5 || abs(j-double2grid_y(data.robot.posi.y))>5)
				// {
				// 	data.stress_parm_grid[i][j] = data.dSum_score[i][j] + calDistance(goal, data.robot.posi)/1000 * 10 + 5;
				// }

				// if(i==double2grid_x(data.robot.posi.x) && j==double2grid_y(data.robot.posi.y))
				// {
				// 	data.stress_parm_grid[double2grid_x(data.robot.posi.x)][double2grid_y(data.robot.posi.y)] -= 3;
				// }

				// if(abs(i-double2grid_x(data.robot.posi.x))<5 && abs(j-double2grid_y(data.robot.posi.y))<5)
				// {
				// 	data.stress_parm_grid[double2grid_x(data.robot.posi.x)][double2grid_y(data.robot.posi.y)] -= 1.0/5;
				// }
				// PRINT("%d  %d  %lf", double2grid_x(data.robot.posi.x), double2grid_y(data.robot.posi.y), data.stress_parm_grid[double2grid_x(data.robot.posi.x)][double2grid_y(data.robot.posi.y)]);


				// パーソナルスペースを侵略していないかを確認
				bool close_to_human_flag = false;
				for(int n=0; n<data.human_num; n++)
				{
					double d = calDistance(data.human[n].posi, (DATA_xy){grid2double_x(i), grid2double_y(j)});
					if(d <= attachment2distance_tmp(pa_human[n].ata))
					{
						close_to_human_flag = true;
						break;
					}
				}

				// パーソナルスペースの外の領域から目的地候補を選び更新
				if(!close_to_human_flag && data.stress_parm_grid[i][j] < min.value)
				{
					min.grid = {i, j};
					min.value = data.stress_parm_grid[i][j];
				}
				
				//ロボットの通りうるパーソナルスペース外の領域の中でストレスポテンシャルの最大、最小の極値を選ぶ
				if(!close_to_human_flag && (data.dMiss_score[i][j] + data.dAnxiety_score[i][j]) > max_stress_potential)
				{
					max_pote = {grid2double_x(i), grid2double_y(j)};
					max_stress_potential = data.dMiss_score[i][j] + data.dAnxiety_score[i][j];
				}

				if(!close_to_human_flag && (data.dMiss_score[i][j] + data.dAnxiety_score[i][j]) < min_stress_potential)
				{
					min_pote = {grid2double_x(i), grid2double_y(j)};
					min_stress_potential = data.stress_parm_grid[i][j];
				}
				
			}
		}

		data.tmp_goal = {grid2double_x(min.grid.i), grid2double_y(min.grid.j)};
		// PRINT("%d, %d, %lf", min.grid.i, min.grid.j, min.value);
		// PRINT("%lf", min.value);

		// explore placeのデバッグ
		// PRINT("ee: %lf  %lf", parm.NextExplorePerPlace2_decrement((DATA_xy){0,1500}, &data.unknown_area_finer), parm.dExplorePlace2((DATA_xy){0,1500}, &data.unknown_area_finer));



		// 顔を向ける場所決定
		DATA_xy look_posi;



		// 目的地リストを作る
		std::vector<DATA_xy> temp_posi;
		for(int i=0; i<data.human_num; i++) temp_posi.push_back(data.human[i].posi);
		// オーナーが部屋から出たのであれば、残像も追加
		// if(data.human_leave_location != nullptr)
		// {
		// 	// temp_posi.push_back(*data.human_leave_location);
		// }
		if(!exist_judge_robot(data.human[0].posi))
		{
			temp_posi.push_back((DATA_xy){3000, 3000});
		}


		// 目的地リストまでの距離を計算
		std::vector<double> temp_dis;
		for(int i=0; i<temp_posi.size(); i++) temp_dis.push_back(calDistance(data.tmp_goal, temp_posi[i]));

		// 最も近い目的地を割り出す
		std::vector<double>::iterator temp_max = std::min_element(temp_dis.begin(), temp_dis.end());
		int n = std::distance(temp_dis.begin(), temp_max);

		// 「至近距離」の設定
		std::vector<double> close_distance(2);
		close_distance[0] = attachment2distance_tmp(data.pa_human[0].ata) * 1.2;
		close_distance[1] = attachment2distance_tmp(data.pa_human[1].ata) * 1.2;
		// close_distance[2] = attachment2distance_tmp(data.pa_human[0].ata) * 1.2;

		// 最も近い人が、至近距離なら、その人を向くように、フラグと場所を変更
		bool look_flag = false;
		if(calDistance(data.tmp_goal, temp_posi[n]) < close_distance[n])
		{
			look_flag = true;
			look_posi = temp_posi[n];
			behavior_id = 1;

			if(n==0)
			{
				if(data.pa_human[1].anx > 0.01 ||  data.pa_human[0].mis > 0.01)
				{
					if(data.pa_human[0].mis > data.pa_human[1].anx)
					{
						behavior_id = EXPLORE;
					}
					else
					{
						behavior_id = EXPLORE;
					}
				}
				else
				{
					if(data.last_MorA==0)
					{
						behavior_id = EXPLORE;
					}
					else
					{
						behavior_id = EXPLORE;
					}
				}
			}
			else if(n==1)
			{
				behavior_id = EXPLORE;
			}
			else if(n==2)
			{
				behavior_id = EXPLORE;
			}
		}
		else
		{
			behavior_id = EXPLORE;
		}

		// behavior_id = 1;
	}

	//////////////////////////////////////////////////////////////////////////////
	// リラックスモードにおける行動選択：能動的なボール遊び、GoToStranger、GoHome
	//////////////////////////////////////////////////////////////////////////////

	if(mode==3)
	{
		behavior_id = HOME;
	}



	if(data.home)
	{
		behavior_id = HOME;
	}

	return behavior_id;




	// 客観的に見てストレスが高い（ストレスの絶対値が閾値以上である）ときは、ストレスを減らすふるまいをする、というのが基本発想
	// とはいっても、ownerがいなかったり、邪魔な場所にstrangerがいたりで、減らしたくても減らせない場合はある
	// かといって、上で算出した「ストレスを減らす勢い」だけで判断できるかといえば、ストレスの絶対値が0でそれ以上減らせない場合だってある

	// そこで、上で算出した「ストレスを減らす勢い」を掛け合わせ、「ストレスを減らすふるまいを選択する価値」を数値化し、
	// それが閾値以上のとき＝これからのふるまいがストレスを減らすために一定の効果をもたらすことが予想されるとき、ストレスを減らすふるまいをする
	// し、そもそもどんなふるまいをすることが良いのかを判断する（行動選択をする）ためにも用いる

	// [ストレスを減らすふるまいを選択する価値] = [減らしたいストレスの絶対値] * [ストレスを減らす勢い]

	/*
	// ところで、ここでいう「ふるまい」とは広義なもので、「何らかのストレスを下げる効果のある動作」のことをいう
	// 犬の愛着行動からは人と犬の位置関係が大事だということがわかっているので、ここでいう「ふるまい」は「移動」を区別するものとなる
	// 「移動」を区別するには、その目的地対象を判断する

	// ロボットは「移動」することによって自身のストレスを下げるが、それは人もまた知覚するものであって、明確に意図を伝える必要がある
	// 単に「ここにくればいろんなストレスが解消されるから」では、人に意図は伝わらない
	// そこで、「ここにくる」要因となったストレスが複数あったとしても、その中で最も支配的なストレスが何かを算出し、それをふるまいを通して表出することが有用となる

	// たとえば、「オーナー」という目的地から、より具体的な「オーナーの前方」「回り込み」「普通に近づくだけ」を選択する
	// これによって、ロボットが「ここにくる」というふるまいが、人には
	// 「あいさつにくる：greeting」「助けを求めに来る：awayfromstranger」「目立った要因はないがこっちの方が安心だから来る：gotoowner」と理解されることを狙う

	// ふるまい選択：大まかな目的地対象（オーナーかストレンジャーか領域か）は、ロボットのストレスを下げるために、全ストレスを総合的に鑑みて決める
	// 　　動作生成：目的地周辺の最終到達点・経路・移動速度・首の向きなど動作の詳細は、人に行動の意図を明確に伝えるために、支配的なストレスに応じて決める
	// （プログラムを書くときの注意点：回り込みか前方かによって、ポテンシャルの最低点を変えているが、それは目的地を変えるためではなく、経路を変えるためである）

	// gotoownerのときの詳細な目的地区別
	{
		if(data.pa_human[0].miss.value > data.pa_human[1].anxiety.value && data.pa_human[0].miss.value > 60)
		{
			// miss由来のふるまい：missが高いなら、特に構って欲しいということなので「オーナーの前方」
		}
		else if(data.pa_human[0].miss.value < data.pa_human[1].anxiety.value && data.pa_human[0].anxiety.value > 60)
		{
			// anxiety由来のふるまい：anxietyが高いなら、特に不安ということなので「回り込み」
		}
		else
		{
			// 上記条件に当てはまらないときは「普通に近づくだけ」
		}
	}
	*/



	// なつき度によるふるまい（なつき度を上げるようなふるまい）
	// --> ストレスがないので、あとはもう、自由気ままにホームで呑気に暮らすか、ロボットの好みの人に能動的にはたらきかけて遊んで仲良くする
	{

	}


	// 変化量が大きいものを選ぶ（本来はソートでやりたい）
	// --> これだと、人のところ以外には行けない（変化量そのものは、行動特性パラメータの絶対値を反映しないから）
	// if(d_score[0] > d_score[1]) motion_id = _GO_TO_OWNER;
	// else motion_id = _EXPLORE_NON_OWNER;
	// // else motion_id = PASSIVE_BEHAVIOR;



	// ふるまいとストレスパラメータが全て一対一で結びついている（たとえば、explore_personがgreeting_personと結びつくように）のなら、
	// はじめから全ストレス値を比較し、大きいものから順に減らすような行動をとればよい
	// しかし、実際は「オーナーのもとへ行く」というふるまいはmissing、anxietyという2つのパラメータと結びついているため、そのようなことはできない
	// そこで、「何らかのストレスを下げる効果があるか」というフレームで動作を切り出し、その動作を行うことによる「ストレスを減らす勢い」を算出する

	// 3つのストレスについて、ownerのもとへ行った場合と、strangerのもとに行った場合の変化量を予測する
	// - これらの値を単体同士で比較する：とにかく、目の前のストレスを順番に減らす
	// - これらの値を足した上で比較する：総合的に考えて、ストレスに苛まれないように生きる

	// 足し合わせたほうがいい気がする
	// - ロボットのストレスが一つということはないから
	// - でも、ロボットの意図を人に伝えるという意味では、代表的なストレスを人に伝える必要はある
	// - そこで、以下のように、同じ「オーナーへ向かう」だとしても、それがどんなストレスに主に支配されているかによって、詳細を区別する
	//  --> でも、それがふるまいなんじゃないの？

	// - 研究のコンセプトは、如何にして移動ロボットがその移動という機能で


	// 変化量が大きいものを選ぶ（本来はソートでやりたい）
	// --> これだと、人のところ以外には行けない（変化量そのものは、行動特性パラメータの絶対値を反映しないから）
//	if(d_socore_ratio[0] > d_socore_ratio[1]) motion_id = _GO_TO_OWNER;
//	else motion_id = _EXPLORE_NON_OWNER;
	// else motion_id = PASSIVE_BEHAVIOR;


	// miss    が要因のふるまい：missing, gotodoor, greeting（行動はプログラム的には全部同じ。missingを上昇させるメカニズムにはドア方向のベクトルを加える）
	// anxiety が要因のふるまい：awayfromstranger（gotoowner, gotohome：オーナーがストレンジャーに近いとホームが選ばれる可能性あり)
	// explore が要因のふるまい：explore


	return 1;
}

//ロボットの移動速度を変更するためのストレスポテンシャルへのなつき度フィルター用関数
double BehaviorSelectorClass::attachment_filter(double attachment)
{
	double K_filter = 1.0 + 0.01 * attachment;

	//double K_filter = 0.5 + 0.015 * attachment;

	return K_filter;
}

//ロボットの速度を計算（ポテンシャルの勾配がきついなら出力される値が大きくなる）
 double BehaviorSelectorClass::robot_velocity(DATA_xy subgoal, DATA_xy robotposition, double sensitivity_of_anxiety)
{
	int i, j, k, l;
	double velocity = 0.0;
	double slope = 0.0;

	i = double2grid_x(subgoal.x);
	j = double2grid_y(subgoal.y);
	k = double2grid_x(robotposition.x);
	l = double2grid_y(robotposition.y);

	//現在位置とゴールの間でのストレスポテンシャルフィールドの勾配を求める(人との関係性を示すための振る舞いであるため、人に関係するストレスを使っている)
	slope = (attachment_filter(data.pa_human[0].ata) * data.dMiss_score[i][j] + data.dAnxiety_score[i][j]) - (attachment_filter(data.pa_human[0].ata) * data.dMiss_score[k][l] + data.dAnxiety_score[k][l]);

	if(i != k && j != l)
	{
		//double calcpotential = (abs(data.stress_parm_grid[i][j] - data.stress_parm_grid[k][l])) / sqrt(pow((i-k),2)+pow((j-l),2));

		double calcpotential = (abs(slope)) / sqrt(pow((i-k),2)+pow((j-l),2));

		velocity = 0.6 + 0.6 * std::tanh(calcpotential/ 5);

	}
	else
	{
		velocity = 0.6;
	}

	return velocity;
}
