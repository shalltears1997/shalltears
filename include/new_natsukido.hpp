#pragma once

#include "c_struct_position.hpp"


class calcDeltaATA
{

private:

	// ownerへのなつき度変化
	double K = 0.0;  // ゲイン
	double T = 1.0;  // 最終値の63.2%に達する時間

	//strangerへのなつき度変化
	double K_stranger = 0.0;
	double T_stranger = 1.0;
	double source_of_T_stranger = 0.0;

	//計算に使うなつき度
	double ata = 0.0;

	//次のなつき度
//	double next_ata = 0.0;

	//なつき度変化判定の関数が結果を返すためのフラグ
	bool judge = false;

	//なつき度発生開始時間
	std::chrono::system_clock::time_point time_ini;

	//なつき度変化判定の継続時間
	std::chrono::system_clock::time_point time_cont;

	//なつき度発生開始時間
	std::chrono::system_clock::time_point time_ini2;

	//なつき度変化判定の継続時間
	std::chrono::system_clock::time_point time_cont2;

	// 顔判定
	bool face_judge = false;
	int hfdpre = 0;

	// しゃがんだ判定
	bool sitting_judge = false;
	double prez = 0;

	// 距離判定
	bool approach_judge = false;
	double predis = 0;
	Position prehumanpose = { 0, 0, 0, 0 };
	Position prerobotpose = { 0, 0, 0, 0 };
	bool moving_to_dog_pre = false;
	bool moving_to_dog = false;

	//接近判定につかう変数
	double prerobot_posix = 0.0;
	double prerobot_posiy = 0.0;
	Vec distance = InitVec();
	Vec rvelocity = InitVec();
	Vec hvelocity = InitVec();
	double prehuman_posix = 0.0;
	double prehuman_posiy = 0.0;
	bool pre_approach = false;

	//人の過度な接近の判定に使う変数
	bool attackness_judge = false;
	Vec distance2 = InitVec();
	std::chrono::system_clock::time_point time_begin;
	std::chrono::system_clock::time_point time_end;

	//社会的参照に用いる変数
	bool social_reference_judge = false;
	double predis_people = 0;
	Position prehuman0pose = { 0, 0, 0, 0 };
	Position prehuman1pose = { 0, 0, 0, 0 };
	bool moving_to_stranger_pre = false;
	bool moving_to_stranger = false;
	Vec distance_people = InitVec();
	Vec st_velocity = InitVec();
	Vec ow_velocity = InitVec();
	std::chrono::system_clock::time_point time_begin2;
	std::chrono::system_clock::time_point time_end2;

	//最終的なフラグ
	bool all_judge = false;

	bool all_judge2 = false;

	//計算に使う定数
	double source_of_T = 0;

	//人の近づいた距離判定関数
	bool calchumanmove(double human_position_x, double human_position_y, double robot_x, double robot_y);

	//人のZ座標判定関数
	bool calchumanZ(double human_z);

	//人の顔の向きの判定関数
	bool calchumanface(int direction);
	
	//社会的参照を行うための関数
	bool social_reference(double human0_position_x, double human0_position_y, double human1_position_x, double human1_position_y);

	//人の過度な接近によるなつきど低下フラグのための関数
	bool attackness(double robot_x, double robot_y, double human_position_x, double human_position_y, double attach);

public:

	// コンストラクタ
	calcDeltaATA(){};

	//なつき度を計算し、変化させるための関数（owner用）
	double calcDeltaAttachment(double pre_attach, double position_x, double position_y, double robot_x, double robot_y, double height, int face_direction);

	//なつき度を計算し、変化させるための関数（stranger用） 分けているのはownerの変化計算にない社会的参照を用いているため 今後はそれぞれで変化の条件が分かれそうなので
	double calcDeltaAttachment_stranger(double pre_attach, double human0_position_x, double human0_position_y, double human1_position_x, double human1_position_y , double robot_x, double robot_y, double height, int face_direction);

};
