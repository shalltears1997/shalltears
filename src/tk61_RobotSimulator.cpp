#include <iostream>
#include <cmath>
#include <chrono>
#include <thread>
#include <fstream>

#include "gv_extern.hpp"
#include "l_tk-library.hpp"
#include "tk51_RobotSimulator.hpp"

RobotSimulatorClass::RobotSimulatorClass(int speed_times_in)
{
	speed_times = speed_times_in;
}

// サブゴールに基づいて、ロボットを動かす（実機で滑らかに描画）
void RobotSimulatorClass::loop()
{
	// 速度のための「距離」は、Pioneerで再び「速度」となってロボットを動かす
	// 「速度」の単位は mm/s であるため、つまり、「距離」を移動するには 1000ms かかることを意味する
	// したがって、サブゴール1つ分の「距離」を divide_num 分割したのだから、1000/divide_num だけSleepすることになる
  //
	// ロボットシミュレータの更新周期を指定する：0 < T < 100（ms）
	double one_period_duration = 5;

	while(thread_continue_flag)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds((int)one_period_duration));
		// if(calDistance(robot.posi, goal_pos) > 100) robotMove();
		if(data.robot_move_or_halt) robotMove();
		bodyRotate();
		pantiltRotate();
		robot_pre = robot;
	}

	std::cout << "the robot simulator thread finished its task." << std::endl;
}

void RobotSimulatorClass::moveLoopThreadBegin()
{
	thread_1 = new std::thread(&RobotSimulatorClass::loop, this);
}

void RobotSimulatorClass::moveLoopThreadFinish()
{
	if(thread_1==nullptr)
	{
		std::cout << "the thread instance had not activated." << std::endl;
	}
	else
	{
		thread_continue_flag = false;
		thread_1->join();
		thread_1=nullptr;
		std::cout << "the thread instance has successfully removed." << std::endl;
	}
}

RobotSimulatorClass::~RobotSimulatorClass()
{
	moveLoopThreadFinish();
}

void RobotSimulatorClass::sendData(DATA_xy subgoal_in, COMMAND_set command_in)
{
	// データ送信（このクラスに送信、このクラスから見たら受信）
  goal_pos.x = subgoal_in.x;
  goal_pos.y = subgoal_in.y;

	// if(calDistance(goal_pos, subgoal_pos) > 100) goal_pos = subgoal_pos;

	// 本当はコマンドじゃなくて、これをもとに計算された指令値（サブゴール）を受け取る
  command.body.deg = command_in.body.deg;
	command.pan.deg = command_in.pan.deg;
  command.tilt.deg = command_in.tilt.deg;
	command.robot.speed = command_in.robot.speed;

	// データを受け取った時刻をその動作の開始時刻として管理
	time_ini = std::chrono::system_clock::now();
	robot_ini_position = robot.posi;
}


void RobotSimulatorClass::robotMove()
{
	// 速度のための「距離」は、Pioneerで再び「速度」となってロボットを動かす
	// 「速度」の単位は mm/s であるため、つまり、「距離」を移動するには 1000ms かかることを意味する
	double speed_per_time = 1000;

	// 指令値の極座標系への変換
	double angle_sim = calDegree(robot.posi, goal_pos);
	double distance_sim = calDistance(robot.posi, goal_pos);

	// サブゴールが与えられてからの経過時間
	std::chrono::system_clock::time_point time_now = std::chrono::system_clock::now();
	double movement_duration = std::chrono::duration_cast<std::chrono::milliseconds>(time_now - time_ini).count();

	// 現在時刻の、全体に対する割合
	double sim_rate = (movement_duration / speed_per_time) * speed_times;
	if(sim_rate > 1) sim_rate = 1;

	// サブゴールが与えられてからの経過時間に応じたロボットの位置算出
	if(movement_duration < speed_per_time)
	{
		robot.posi.x = robot_ini_position.x + distance_sim * sim_rate * cos(angle_sim*PI/180) /* 0.3*/;
		robot.posi.y = robot_ini_position.y + distance_sim * sim_rate * sin(angle_sim*PI/180) /* 0.3*/;
	}
}

void RobotSimulatorClass::bodyRotate()
{
	// ロボット本体角度
	// if(command.robot.speed > 10)
	if(data.robot_move_or_halt)
	{
		// 移動中＝角度は必然的に決まる
		robot.body_deg = calDegree(robot_pre.posi, robot.posi);
	}
	else
	{
		// 停止中＝指定された角度に回転
		robot.body_deg = command.body.deg;
	}
}

void RobotSimulatorClass::pantiltRotate()
{
	robot.pan_deg = command.pan.deg;
	robot.tilt_deg = command.tilt.deg;
}

void RobotSimulatorClass::receiveDataLocation(DataClass *data_in)
{
  data_in->robot.posi.x = robot.posi.x;
  data_in->robot.posi.y = robot.posi.y;
}

void RobotSimulatorClass::receiveDataDeg(DataClass *data_in)
{
  data_in->robot.body_deg = robot.body_deg;
	data_in->robot.pan_deg = robot.pan_deg;
  data_in->robot.tilt_deg = robot.tilt_deg;
}
