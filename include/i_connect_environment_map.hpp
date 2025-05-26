#pragma once

#include <iostream>
#include <thread>
#include <c_struct_position.hpp>
#include <tk_data_class.hpp>

class connectWithEMClass
{
private:

  int ser_Pio, acc_Pio, ser_EM, acc_EM1, acc_EM2, ser_Pan, acc_Pan, ser_PF, acc_PF, send_cnt1,recv_cnt1,ser_tale, acc_tale, ser_lrf, acc_lrf;
  int cri1,cri2,cri3;
  int ser_Deg = 0;
  unsigned short PORT_Deg = 45680;
  int acc_Deg = 0;
  const int num_of_recv_pio = 50;

  // 非公開メソッド
  int sendData();  // 一回きりの送信
  int receiveWeight();  // 一回きりの受信
  int receiveCoordinate();
  void connectLoop();  // スレッドから呼ぶ
  void objectloop();

  // 内部変数
  bool robot_move_or_halt = false;

  // スレッド
  std::thread *thread_1;
  std::thread *thraed_2;
  bool thread_continue_flag = true;

  // メインスレッドで値が更新されるとこれがtrueになるので、送受信をする
  bool *is_new;

  int send_coordinate[4]={};

public:

  // コンストラクタ
  connectWithEMClass(){};

  void objectloopbegin();
  // 受信メソッド
  void connectLoopThreadBegin(bool *is_new);  // ループ受信のスレッドを開始
  void connectLoopThreadFinish();  // ループ受信のスレッドを終了

  // デストラクタ
  ~connectWithEMClass();  // スレッドを終了する
};
