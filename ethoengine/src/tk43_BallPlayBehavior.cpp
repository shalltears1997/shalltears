#include <iostream>
#include <cstdlib>
#include <thread>

#include "gv_extern.hpp"
#include "c_struct_position.hpp"
#include "l_tk-library.hpp"
#include "tk33_BallPlayBehavior.hpp"

void MotionClass::set_parm(DataClass all_data_in)
{
	d = all_data_in;
}


// 0. ボールを探すためにキョロキョロする
void BallPlay_BC::ballLookAround()
{
	// 本体
	// command.robot.path = GeneratePotePathToGoal({-2500, 500});
	command.robot.speed = 0;
	// 本体・回転
	command.body.deg = data->robot.body_deg;
	command.body.speed = 20;
  // 頭部角度参照元（0：EE、1：ボール）
  command.head_flag = 0;
	// 頭部・パン
	// 5秒かけて -30〜30 = 60[deg] を移動させるには、
	// 1回の割り込み（20ms）で 60/(5/0.02)=0.24[deg] 移動させれば良い
	if (data->pan > 45) rotation_pan = -1;
	if (data->pan <-45) rotation_pan = +1;
	command.pan.deg = panSetRelativeAngle(data->pan + rotation_pan * 3);
	// command.pan.speed = 20;
	// 頭部・チルト
	command.tilt.deg = -30;
	command.tilt.speed = 20;
	// グリッパ
	command.gripper.deg = gripperOpen();
	command.gripper.speed = 20;
}


// 1. ボールの元へ移動、カメラは反射に任せる、グリッパ開く
void BallPlay_BC::ballChase()
{
	// 本体
	command.robot.path = GeneratePotePathToGoal(data->ball_tk);
	command.robot.speed = 120;  // 速度を落とす
	// 本体・回転
	command.body.deg = bodyRotateTo(data->ball_tk);
	command.body.speed = 20;
  // 頭部角度参照元（0：EE、1：ボール）
  command.head_flag = 1;
	// 頭部・パン
	command.pan.deg = panRotateTo(data->ball_tk);
	command.pan.speed = 20;
	// 頭部・チルト
	command.tilt.deg = tiltRotateTo(data->ball_tk);
	command.tilt.speed = 20;
	// グリッパ
	command.gripper.deg = gripperOpen();
	command.gripper.speed = 20;
}


// 2. ボールの元へ移動、カメラは反射に任せる、グリッパ閉じる
void BallPlay_BC::ballCatch()
{
	// 本体
	command.robot.path = GeneratePotePathToGoal(data->ball_tk);
	command.robot.speed = 100;  // 止まるギリギリまで速度を落とす（が、停止してしまうとボールが遠ざかっちゃうと思うのでそうはしない）
	// 本体・回転
	command.body.deg = data_pre.command.body.deg;
	command.body.speed = 20;
  // 頭部角度参照元（0：EE、1：ボール）
  command.head_flag = 1;
	// 頭部・パン
	command.pan.deg =data_pre.command.pan.deg;
	command.pan.speed = 20;
	// 頭部・チルト
	command.tilt.deg = tiltRotateTo(data->ball_tk);
	command.tilt.speed = 20;
	// グリッパ
	command.gripper.deg = gripperClose();
	command.gripper.speed = 20;
}


// 3. オーナーの元へ移動する
void BallPlay_BC::ballCarry()
{
	// 本体
	command.robot.path = GeneratePotePathToGoal(data->human[0].posi);
	command.robot.speed = 150;  // 速度を落とす（ボールを持っているので慎重に、ということで）
	// 本体・回転
	command.body.deg = bodyRotateTo(data->human[0].posi);
	command.body.speed = 20;
  // 頭部角度参照元（0：EE、1：ボール）
  command.head_flag = 0;
	// 頭部・パン
	command.pan.deg = panRotateTo(data->human[0].posi);
	command.pan.speed = 20;
	// 頭部・チルト
	command.tilt.deg = tiltRotateTo(data->human[0].posi);
	command.tilt.speed = 20;
	// グリッパ
	command.gripper.deg = gripperClose();
	command.gripper.speed = 20;
}


// 4. 静止してボールを離す
void BallPlay_BC::ballRelease()
{
	// 本体
	command.robot.path = GeneratePotePathToGoal(data->human[0].posi);
	command.robot.speed = 0;
	// 本体・回転
	command.body.deg = bodyRotateTo(data->human[0].posi);
	command.body.speed = 20;
  // 頭部角度参照元（0：EE、1：ボール）
  command.head_flag = 0;
	// 頭部・パン
	command.pan.deg = panRotateTo(data->human[0].posi);
	command.pan.speed = 20;
	// 頭部・チルト
	command.tilt.deg = tiltRotateTo(data->human[0].posi);
	command.tilt.speed = 20;
	// グリッパ
	command.gripper.deg = gripperOpen();
	command.gripper.speed = 20;

	// 仮処置！！
	// data->ball_tk = {3000, 3500};
	data->ball_tk = data->human[0].posi;

	// data->ball_st.ball_presence = false;
}


////////////////////////////////////////////////////////////////////////////////

// ボール遊びクラス
void BallPlay_BC::behavior()
{
  // 時間管理
	time_now = std::chrono::system_clock::now();
	duration = (double) std::chrono::duration_cast<std::chrono::milliseconds>(time_now - time_ini).count() / 1000;
	action_duration = (double) std::chrono::duration_cast<std::chrono::milliseconds>(time_now - time_action_ini).count() / 1000;

	// 動作指定：遷移モデルに沿って決める

	// 0.探索：首を振る
	//   ボールが見つかるまで
	if(action_num == 0)
	{
    if (data->ball_st.ball_presence) action_num = 1;  // done（正常）：追従へ
		else if(action_duration > 7) action_num = 9;  // done（終了）：完了へ
		else action_num = action_num;  // continue
	}
	// 1.追従：ボールへ移動
	//   ボールまでの距離が小さくなるまで
	else if(action_num == 1)
	{
		if(!data->ball_st.ball_presence) action_num = 0;   // exception -> 探す
		else if(calDistance(data->robot.posi, data->ball_tk) < 50) action_num = 2;  // done -> 掴む
		else action_num = action_num;  // continue
	}
	// 2.掴む：ボールへ移動 & 把持
	//   グリッパを閉じるまで（本来はグリッパの値を参照する／現在は仮で3秒以上経過したら）
  else if(action_num == 2)
	{
    if(!data->ball_st.ball_presence) action_num = 0;  // exception -> 探す
    else if(action_duration > 3) action_num = 3;  // done -> 運ぶ
		else action_num = action_num;  // continue
	}
	// 3.運搬：人へ移動 & 把持
	//   オーナーまでの距離が小さくなるまで
  else if(action_num == 3)
	{
		double dis_th = attachment2distance_tmp(data->pa_human[0].ata) * 1.2;

    if(calDistance(data->robot.posi, data->human[0].posi) < dis_th) action_num = 4;  // done -> 解放
		else action_num = action_num;  // continue
	}
	// 4.解放：静止 & 解放
	//   グリッパを開き、そこから3秒経過するまで
  else if(action_num == 4)
	{
    if(action_duration > 3) action_num = 9;  // done -> 完了
		else action_num = action_num;  // continue
	}
	// 9.完了
	else if(action_num == 9)
	{
    data->ball_play_required_level = 0;
  }

	PRINT("%d", action_num);

	// return;

	// 動作実行
	switch(action_num)
	{
		case 0:  // [例外処理] 3秒間ボールを探す
			ballLookAround();
      data->ball_play_required_level = 100;  // 現動作段階におけるふるまいの継続必要性を指定
			break;

		case 1:  // ボールに近くまで：ボールを追いかける
			ballChase();
      data->ball_play_required_level = 100;
      break;

    case 2:  // 3秒間：ボールを把持する/ボールを追いかけつづける
      ballCatch();
      data->ball_play_required_level = 100;
      break;

    case 3:  // オーナーの元へ到達するまで：オーナーの元へ向かう/ボールを把持する
      ballCarry();
      data->ball_play_required_level = 100;
      break;

    case 4:  // 3秒間：静止してボールを離す
      ballRelease();
      data->ball_play_required_level = 100;
      break;

		default:  // ボール遊びは選択しない
      data->ball_play_required_level = 0;
      break;
	}

  // 動作継続時間管理
  if(action_num_pre != action_num) time_action_ini = std::chrono::system_clock::now();

	// 動作番号管理
	action_num_pre = action_num;
}
