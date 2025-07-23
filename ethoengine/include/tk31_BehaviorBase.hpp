#pragma once

#include <iostream>
#include <cstdlib>
#include <thread>

#include "tk_data_class.hpp"
#include "c_struct_position.hpp"

// 基底クラス
class BehaviorClass
{
public:

	// コンストラクタ（サブクラスからもこれを呼ぶ）
	BehaviorClass(DataClass *all_data_in);

	// 実行
	COMMAND_set act();

	// ポリモーフィズムのために、デストラクタはvirtualで宣言する
	virtual ~BehaviorClass(){};

protected:

	// データ
	DataClass *data;

	// ポリモーフィズムのために、純粋仮想関数（子クラスでの上書きが必須となる）として仮宣言
	// サブクラスでは、これを上書きして、この中にふるまい内容を記述する

	virtual void behavior() = 0;



	// インナー・ライブラリ
	std::vector<DATA_xy> GeneratePotePathToGoal(DATA_xy goal_in);
	DATA_xy human_around(DATA_xy human_target, DATA_xy human_avoid);
	double rotateTo(DATA_xy target);
	DATA_xy fix_object_goal(DATA_xy current_goal);
	DATA_xy fix_object_goal2(DATA_xy current_goal);
	bool Notify_area_judge();
	bool circle_around_obj(DATA_xy robot,DATA_xy goal);
	bool rotate_area(DATA_xy robot,DATA_xy object);
	DATA_xy Between_user_and_object(DATA_xy object);
	DATA_xy Change_goal(DATA_xy pre_goal);
	double bodyRotateTo(DATA_xy target);
	double panRotateTo(DATA_xy target);
	double panSetAbsoluteAngle(double angle);
	double panSetRelativeAngle(double angle);
	double tiltRotateTo(DATA_xy target);
	double gripperOpen();
	double gripperClose();

	COMMAND_set command;


};
