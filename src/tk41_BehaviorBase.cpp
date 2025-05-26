#include <iostream>
#include <cstdlib>
#include <thread>

#include "c_struct_position.hpp"
#include "l_tk-library.hpp"
#include "tk31_BehaviorBase.hpp"

////////////////////////////////////////////////////////////////////////////////
// インナー・ライブラリ
////////////////////////////////////////////////////////////////////////////////

// ゴールから経路を生成するメソッドの呼び出しをまとめたもの
std::vector<DATA_xy> BehaviorClass::GeneratePotePathToGoal(DATA_xy goal_in)
{
	// ポテンシャル生成
  DATA_xy goal = {goal_in.x, goal_in.y};
	PotentialGenerator tmp(goal, data->human, data->pa_human, data->obstacle);
	PotentialFunction pf = tmp;  // 目的地に向かうポテンシャルをデータクラスに代入（デバッグ用）
	std::vector<DATA_grid_part> low_grids = tmp.getLowGrids();  // ポテンシャルの低い場所をデータクラスに代入（デバッグ用）
	data->low_grids = low_grids;

  // 経路生成
	PathGenerator path(pf, low_grids, data->robot.posi);  // 経路を生成するインスタンス
  return path.generate_path();
}


double BehaviorClass::tiltRotateTo(DATA_xy target)
{
	double height_robot = 550;
	double height_target = 1600;

	return atan2(height_target - height_robot, calDistance(data->robot.posi, target)) * 180.0/PI;
}


double BehaviorClass::rotateTo(DATA_xy target)
{
  return calDegree(data->robot.posi, target);
}

// ロボットの現在地から目的地方向角を算出する
double BehaviorClass::bodyRotateTo(DATA_xy target)
{
  return rotateTo(target);
}

// 回り込みの場所を算出
DATA_xy BehaviorClass::human_around(DATA_xy human_target, DATA_xy human_avoid)
{
  // その位置を目的地にするためにずらす：750mm＝なつき度が最も高い時の、最も近い距離
  // なつき度が低い時は斥力で遠くなるので、ここでは決めうちで良い
	double degree = calDegree(human_avoid, human_target) * PI / 180 + PI;
	human_target.x -= 750 * cos(degree);
	human_target.y -= 750 * sin(degree);
	return human_target;
}

//動物体のための目的地修正//Rの値で近さ代えられるよ。
DATA_xy BehaviorClass::fix_object_goal(DATA_xy current_goal)
{
 	DATA_xy pregoal = {};
	DATA_xy goal = {};
 	DATA_xy goal1 = {};
 	DATA_xy goal2 = {};
 	double R = 1500.0;
 	double k = 0.0;

 	k = (data->robot.posi.y - current_goal.y) / (data->robot.posi.x - current_goal.x);
 	goal1.x = current_goal.x + (R / sqrt(1 + k*k));
 	goal1.y = current_goal.y + (R*k / sqrt(1 + k*k));
 	goal2.x = current_goal.x - (R / sqrt(1 + k*k));
 	goal2.y = current_goal.y - (R*k / sqrt(1 + k*k));
 	if(calDistance(data->robot.posi,goal1) < calDistance(data->robot.posi,goal2))goal = goal1;
 	else goal = goal2;
 	if(calDistance(goal,pregoal) < 100)goal = pregoal;

 	pregoal = goal;

 	return goal;
}

DATA_xy BehaviorClass::fix_object_goal2(DATA_xy current_goal)
{
 	DATA_xy pregoal = {};
	DATA_xy goal = {};
 	DATA_xy goal1 = {};
 	DATA_xy goal2 = {};
 	double R = 700.0;
 	double k = 0.0;

 	k = (data->robot.posi.y - current_goal.y) / (data->robot.posi.x - current_goal.x);
 	goal1.x = current_goal.x + (R / sqrt(1 + k*k));
 	goal1.y = current_goal.y + (R*k / sqrt(1 + k*k));
 	goal2.x = current_goal.x - (R / sqrt(1 + k*k));
 	goal2.y = current_goal.y - (R*k / sqrt(1 + k*k));
 	if(calDistance(data->robot.posi,goal1) < calDistance(data->robot.posi,goal2))goal = goal1;
 	else goal = goal2;
 	if(calDistance(goal,pregoal) < 100)goal = pregoal;

 	pregoal = goal;

 	return goal;
}

bool BehaviorClass::Notify_area_judge()
{
	if((-2400 < data->robot.posi.x && data->robot.posi.x <-2200) && (400 < data->robot.posi.y && data->robot.posi.y < 600))
	{
		return true;
	}

	return false;
}

bool BehaviorClass::rotate_area(DATA_xy robot,DATA_xy goal)
{

	PRINT("distance_b:%lf\n",calDistance(robot,goal));


	if(calDistance(robot,goal) < 100)
	{
		return true;
	}
	return false;

}

bool BehaviorClass::circle_around_obj(DATA_xy robot,DATA_xy obj)
{

//	PRINT("distance_a:%lf\n",calDistance(robot,obj));
	if((robot.x-obj.x)*(robot.x-obj.x) + (robot.y-obj.y)*(robot.y-obj.y) < 1000000)
	{
		return true;
	}
	return false;

}

DATA_xy BehaviorClass::Between_user_and_object(DATA_xy object)
{

}

DATA_xy BehaviorClass::Change_goal(DATA_xy pre_goal)
{
	//もらった座標をただただ何度か回転させる処理
}

double BehaviorClass::panRotateTo(DATA_xy target)
{
  return panSetAbsoluteAngle(rotateTo(target));
}

double BehaviorClass::panSetAbsoluteAngle(double angle)
{
  angle -= data->robot.body_deg;
  if(angle > 180) angle -= 360;
  if(angle <-180) angle += 360;
  return angle;
}

double BehaviorClass::panSetRelativeAngle(double angle)
{
  return angle;
}

double BehaviorClass::gripperOpen()
{
  return 0;
}

double BehaviorClass::gripperClose()
{
  return 100;
}

////////////////////////////////////////////////////////////////////////////////
// 基底クラスのコンストラクタ
////////////////////////////////////////////////////////////////////////////////

// 全ふるまいに共通する規定動作を定義する
BehaviorClass::BehaviorClass(DataClass *all_data_in)
{
  data = all_data_in;
}

////////////////////////////////////////////////////////////////////////////////
// 呼び出しメソッド
////////////////////////////////////////////////////////////////////////////////

// ふるまいを実行する
COMMAND_set BehaviorClass::act()
{
  // 初期化
	// 本体
	command.robot.path = GeneratePotePathToGoal({0, 1500});
	command.robot.speed = 0;
	// 本体・回転
	command.body.deg = 0;
	command.body.speed = 20;
  // 頭部角度参照元（0：EE、1：ボール）
  command.head_flag = 0;
	// 頭部・パン
	command.pan.deg = 0;
	command.pan.speed = 0;
	// 頭部・チルト
	command.tilt.deg = 0;
	command.tilt.speed = 0;
	// グリッパ
	command.gripper.deg = 0;
	command.gripper.speed = 0;

  // 動作上書き
  behavior();

	return command;
}
