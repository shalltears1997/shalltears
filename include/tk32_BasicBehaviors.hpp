#pragma once

#include <iostream>
#include <cstdlib>
#include <thread>

#include "tk_data_class.hpp"
#include "c_struct_position.hpp"
#include "tk31_BehaviorBase.hpp"

////////////////////////////////////////////////////////////////////////////////
// 基底ふるまいクラスを継承し、下に各ふるまいごとのクラスを宣言する
// 定義は.cppファイルにて、behavior() メソッド内に記述する
////////////////////////////////////////////////////////////////////////////////
// 新たなふるまいを作る場合
//    - ここでは、既存の宣言をコピーし、クラス名とコンストラクタを変更すれば良い
//    - 内容は.cppファイルにて、behavior() メソッド内に記述する
////////////////////////////////////////////////////////////////////////////////

class Home_BC : public BehaviorClass
{
public:
	Home_BC(DataClass *data_in) : BehaviorClass(data_in){};
	void behavior();
	~Home_BC(){};
};


class Halt_BC : public BehaviorClass
{
public:
	Halt_BC(DataClass *data_in) : BehaviorClass(data_in){};
	void behavior();
	~Halt_BC(){};
};

///////

Position TargetDecision(bool cognition);

///////

class SoundGaze_BC : public BehaviorClass
{
public:
	SoundGaze_BC(DataClass *data_in) : BehaviorClass(data_in){};
	void behavior();
	~SoundGaze_BC(){}
};


class SoundMove_BC : public BehaviorClass
{
public:
	SoundMove_BC(DataClass *data_in) : BehaviorClass(data_in){};
	void behavior();
	~SoundMove_BC(){}
};


class Exploring_BC : public BehaviorClass
{
public:
	Exploring_BC(DataClass *data_in) : BehaviorClass(data_in){};
	void behavior();
	~Exploring_BC(){}
};


class AttentionGetting_BC : public BehaviorClass
{
public:
	AttentionGetting_BC(DataClass *data_in) : BehaviorClass(data_in){};
	void behavior();
	~AttentionGetting_BC(){}
};


class ShowingDirection_BC : public BehaviorClass
{
public:
	ShowingDirection_BC(DataClass *data_in) : BehaviorClass(data_in){};
	void behavior();
	~ShowingDirection_BC(){}
};



class GazeAlternation_BC : public BehaviorClass
{
public:
	GazeAlternation_BC(DataClass *data_in) : BehaviorClass(data_in){};
	void behavior();
	~GazeAlternation_BC(){}
};


class LeadingBehavior_BC : public BehaviorClass
{
public:
	LeadingBehavior_BC(DataClass *data_in) : BehaviorClass(data_in){};
	void behavior();
	~LeadingBehavior_BC(){}
};


class GoToDoor_BC : public BehaviorClass
{
public:
	GoToDoor_BC(DataClass *data_in) : BehaviorClass(data_in){};
	void behavior();
	~GoToDoor_BC(){}
};


class SoundBehavior_BC : public BehaviorClass
{
public:
	SoundBehavior_BC(DataClass *data_in) : BehaviorClass(data_in){};
	void behavior();
	~SoundBehavior_BC(){}
};


class GoToOwner_BC : public BehaviorClass
{
public:
	GoToOwner_BC(DataClass *data_in) : BehaviorClass(data_in){};
	void behavior();
	~GoToOwner_BC(){}
};


// class Missing_BC : public BehaviorClass
// {
// public:
// 	Missing_BC(DataClass *data_in) : BehaviorClass(data_in){};
// 	void behavior();
// ~Missing_BC(){}
// };

class Missing_BC : public BehaviorClass
{
public:
	Missing_BC(DataClass *data_in) : BehaviorClass(data_in){};
	void behavior();
	~Missing_BC(){}
};


class Play_BC : public BehaviorClass
{
public:
	Play_BC(DataClass *data_in) : BehaviorClass(data_in){};
	void behavior();
	~Play_BC(){}
};


class Greets_BC : public BehaviorClass
{
public:
	Greets_BC(DataClass *data_in) : BehaviorClass(data_in){};
	void behavior();
	~Greets_BC(){}
};


class GreetsToStranger_BC : public BehaviorClass
{
public:
	GreetsToStranger_BC(DataClass *data_in) : BehaviorClass(data_in){};
	void behavior();
	~GreetsToStranger_BC(){}
};


class GreetsToStranger2_BC : public BehaviorClass
{
public:
	GreetsToStranger2_BC(DataClass *data_in) : BehaviorClass(data_in){};
	void behavior();
	~GreetsToStranger2_BC(){}
};


class ExploreStranger_BC : public BehaviorClass
{
public:
	ExploreStranger_BC(DataClass *data_in) : BehaviorClass(data_in){};
	void behavior();
	~ExploreStranger_BC(){}
};


class AttachmentBehavior_BC : public BehaviorClass
{
public:
	AttachmentBehavior_BC(DataClass *data_in) : BehaviorClass(data_in){};
	void behavior();
	~AttachmentBehavior_BC(){}
};


class PassiveBehavior_BC : public BehaviorClass
{
public:
	PassiveBehavior_BC(DataClass *data_in) : BehaviorClass(data_in){};
	void behavior();
	~PassiveBehavior_BC(){}
};


class PlayUsingBall_BC : public BehaviorClass
{
public:
	PlayUsingBall_BC(DataClass *data_in) : BehaviorClass(data_in){};
	void behavior();
	~PlayUsingBall_BC(){}
};

class Monitoring_Obj_BC : public BehaviorClass
{
public:
	Monitoring_Obj_BC(DataClass *data_in) : BehaviorClass(data_in){};
	void behavior();
	~Monitoring_Obj_BC(){}
};


class Monitoring_Area_BC : public BehaviorClass
{
public:
	Monitoring_Area_BC(DataClass *data_in) : BehaviorClass(data_in){};
	void behavior();
	~Monitoring_Area_BC(){}
};


class Monitoring_User_BC : public BehaviorClass
{
public:
	Monitoring_User_BC(DataClass *data_in) : BehaviorClass(data_in){};
	void behavior();
	~Monitoring_User_BC(){}
};


class Notify_BC : public BehaviorClass
{
public:
	Notify_BC(DataClass *data_in) : BehaviorClass(data_in){};
	void behavior();
	~Notify_BC(){}
};

class Rotate_BC : public BehaviorClass
{
public:
	Rotate_BC(DataClass *data_in) : BehaviorClass(data_in){};
	void behavior();
	~Rotate_BC(){}
};
