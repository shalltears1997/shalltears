#pragma once

#include <iostream>
#include <thread>
#include <c_struct_position.hpp>
#include <tk_data_class.hpp>

class connectWithMCClass
{
private:

  int ser_Pio, acc_Pio, ser_Pan, acc_Pan, ser_PF, acc_PF, send_cnt, ser_tale, acc_tale, ser_lrf, acc_lrf;
  int ser_Deg = 0;
  unsigned short PORT_Deg = 45680;
  int acc_Deg = 0;
  const int num_of_recv_pio = 50;

  // 非公開メソッド
  int sendData();  // 一回きりの送信
  int receiveData();  // 一回きりの受信
  void connectLoop();  // スレッドから呼ぶ

  // 内部変数
  bool robot_move_or_halt = false;

  // スレッド
  std::thread *thread_1;
  bool thread_continue_flag = true;

  // バッファ（受信用）
  bool catch_flag;
  double ball_distance_0;
  double ball_theta_1;
  double confidence_sum_1;
  double SD_1;
  double circularity_1;
  double robot_deg;

  // バッファ（送信用）
  bool pioneer_activate;
	int motion_id;
  DATA_xy sub_goal;
	double sound_gaze_angle, hark_sd; // とりあえず
	DATA_xy robot;
  COMMAND_set command;
  bool ball_st_hand_flag;

  // メインスレッドで値が更新されるとこれがtrueになるので、送受信をする
  bool *is_new;

public:

  // コンストラクタ
  connectWithMCClass(){};

  // 受信メソッド
  void connectLoopThreadBegin(bool *is_new);  // ループ受信のスレッドを開始
  void connectLoopThreadFinish();  // ループ受信のスレッドを終了

  // データ受け渡しメソッド
  void dataInput(DataClass *data_in);
  void dataOutput(DataClass *data_in);

  // デストラクタ
  ~connectWithMCClass();  // スレッドを終了する
};
