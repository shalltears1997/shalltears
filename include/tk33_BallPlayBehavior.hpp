#pragma once

#include <iostream>
#include <cstdlib>
#include <thread>
#include "gv_extern.hpp"
#include "tk31_BehaviorBase.hpp"

class MotionClass
{
public:
	// ローカル変数としての全情報
	DataClass d;

	MotionClass(){};

	void set_parm(DataClass all_data_in);

	COMMAND_set goTohuman_simple();
	COMMAND_set goToNONhuman_simple();
	COMMAND_set ballChase();
	COMMAND_set ballLookAround();
	COMMAND_set ballCatch();
  void ballCarry(){}
  void ballRelease(){}
};




class BallPlay_BC : public BehaviorClass
{
private:

	// 動作
	void ballChase();
	void ballLookAround();
	void ballCatch();
  void ballCarry();
  void ballRelease();

	// 首振りの向きを管理する（初期値は1か-1にしておく）
	int rotation_pan = 1;

  // 継続時間
	std::chrono::system_clock::time_point time_ini;
	std::chrono::system_clock::time_point time_now;
	double duration = 0;

	// 動作指定
	std::chrono::system_clock::time_point time_action_ini;
	double action_duration = 0;
	void setAction(int action_num);
	double getActionDuration();
	int action_num_pre = 0;
	int action_num = 0;

public:

	BallPlay_BC(DataClass *data_in) : BehaviorClass(data_in)
	{
		time_ini = std::chrono::system_clock::now();
		time_action_ini = time_ini;
	}

	void behavior();
	~BallPlay_BC(){}
};
