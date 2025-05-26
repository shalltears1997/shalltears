#pragma once

#include <chrono>
#include "c_struct_position.hpp"
#include "l_tk-library.hpp"

class RobotSimulatorClass
{
private:

  // 時間管理
	std::chrono::system_clock::time_point time_ini;
	DATA_xy robot_ini_position;

  // スレッド
  std::thread *thread_1;
  bool thread_continue_flag = true;

  // 倍速設定
  double speed_times = 1;

  // 受信データ
  DATA_xy subgoal_pos, goal_pos;
  COMMAND_set command;

  // 内部情報
	// DATA_robot robot = {2500, 2500}, robot_pre;
	// DATA_robot robot = {-2500, 2500}, robot_pre;
	DATA_robot robot = (DATA_robot){0, 1500}, robot_pre;


public:

  // コンストラクタ
  RobotSimulatorClass(int speed_times_in);

  // ロボットシミュレータ始動・停止
  void moveLoopThreadBegin();  // ループ動作のスレッドを開始
  void moveLoopThreadFinish();  // ループ動作のスレッドを終了

  // データ送受信
  void sendData(DATA_xy subgoal_in, COMMAND_set command_in);
  void receiveDataLocation(DataClass *data_in);
  void receiveDataDeg(DataClass *data_in);

  void loop();
  void robotMove();
	void bodyRotate();
	void pantiltRotate();

  // デストラクタ
  ~RobotSimulatorClass();  // スレッドを終了する
};
