#pragma once

#include <iostream>
#include <thread>
#include <c_struct_position.hpp>
#include <tk_data_class.hpp>

class connectWithUnityClass
{
protected:

  // 内部変数
  int ser_Uni, acc_Uni, ser_Pan, acc_Pan, ser_PF, acc_PF, send_cnt, ser_tale, acc_tale, ser_lrf, acc_lrf;
  int cri;
  const int NUM_OF_RECV_DATA = 26; //ZPSからの受信データ長 固定
  bool *unity_robot_new;  // 更新フラグへのポインタ

  // 非公開メソッド
  void receiveLoop();  // スレッドから実行する
  double dist(double x, double y);
  double siz(double x, double y);

  // スレッド
  std::thread *thread_1;
  bool thread_continue_flag = true;

  // 受信データ（直接dataに代入するのではなく、ここにためる。バッファのように）
  DATA_xy robot_c;
  DATA_xy human_c[3];
  double human_height[3];

public:

  // コンストラクタで、コロンに続いてthreadインスタンスを初期化・宣言
  //   （std::threadは宣言時にバインドする関数を指定しなくてはいけないので）
  // これを使うには、std::threadの定義はポインタではなく実態で行う
  // connectWithZPSClass() : thread_1(&connectWithZPSClass::receiveThread, this) {}

  // コンストラクタ
  connectWithUnityClass(){};

  // 受信メソッド
  int receive();  // 一回きりの受信
  void receiveLoopThreadBegin(bool *unity_robot_new);  // ループ受信のスレッドを開始
  void receiveLoopThreadFinish();  // ループ受信のスレッドを終了

  // データ受け渡しメソッド
  void dataInput(DataClass *data_in);

  DATA_xy dataInput_robot(){return robot_c;}
  DATA_xy dataInput_human(int human_n);
  // DATA_xy dataInput_human(int human_n){return human_c[human_n];}
  double dataInput_human_height(int human_n){return human_height[human_n];}

  // デストラクタ
  ~connectWithUnityClass();  // スレッドを終了する
};
