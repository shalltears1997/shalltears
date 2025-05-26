#include <iostream>
#include <cstring>
#include <chrono>
#include <thread>
#include <cmath>
#include <cstdio>

#if defined(_WIN32) || defined(_WIN64)
	#include "tcpip_win.hpp"
#else
	#include <sys/types.h>
	#include <sys/socket.h>
	#include "tcpip_uni.hpp"
#endif

#include "class.hpp"
#include "gv_extern_id.hpp"
#include "gv_extern.hpp"

#include "05_connect_mc.hpp"
#include "22_hark.hpp"

#include "05_connect_mc.hpp"

#include <sys/select.h>

int connectWithMCClass::sendData()
{
	const int send_num = 87;
	int num_of_string, return_value;
	char send_message[90] = {};
	
	// データセット
	num_of_string = sprintf(send_message,
		"%07.1f,%1d,%02d,%07.1f,%07.1f,"
		"%2d,%07.2f,%07.2f,%07.1f,%07.1f,%07.1f,%07.1f,%07.1f",
		// robot_move_or_halt, motion_id, sub_goal.x, sub_goal.y,
		sound_flag, robot_move_or_halt, command.head_flag, sub_goal.x, sub_goal.y,
		ball_st_hand_flag, sound_gaze_angle, hark_sd, robot.x, robot.y, command.pan.deg, command.tilt.deg, command.body.deg);
	// PRINT("send_message: %s\n", send_message);
	//%07.1f,/*sound_flag, */

	// データ送信
	send_cnt = send(acc_Pio, send_message, num_of_string, 0);
//PRINT("send_cnt:%d\n",send_cnt);
	// デバッグ用
	// timeInterval = (double)(timeGetTime() - lastTGT)/CLOCKS_PER_SEC;
	//printf("sndMotion: %7.3lf  %7.3lf\n", (double)(timeGetTime() - initialTGT)/CLOCKS_PER_SEC, timeInterval);
	// lastTGT = timeGetTime();

	if(send_cnt == send_num) return_value = send_cnt;
	else return_value = -1;
	return return_value;
}


int connectWithMCClass::receiveData()
{
	char recv_msg[127] = {""}; // 受信データを格納
	int num_of_recv = 0; //受信メッセージ長
	double temp_pose[2]={};
	char* tp;
	char tp_a[72] = "";
	char recv_msg_cp[256] = "";

	//データを受信
	// if(( num_of_recv = recv(acc_Deg, recv_msg, num_of_recv_pio, 0 )) == -1 )
	if(( num_of_recv = recv(acc_Pio, recv_msg, num_of_recv_pio, 0 )) == -1 )
	{ //データ長を固定して送受信する場合
   	perror("Recieve data from ZPS");
   	return -1;
	}

	//受信バッファの確認
	//PRINT("%s\n", recv_msg);

	//バッファからデータを取り出し，仮の配列に代入
	strcpy(recv_msg_cp, recv_msg);
	tp = strtok(recv_msg, "#"); //pioneer 速度情報
	if(tp != NULL){
		strcpy(tp_a, tp);
		velocity = atof(tp_a);
	}
	tp = strtok(recv_msg, "#"); //pioneer 角度情報
	if(tp != NULL){
		strcpy(tp_a, tp);
		robot_deg = atof(tp_a);
	}
	tp = strtok(NULL, "#"); //camera 情報のID
	if(tp != NULL){
		strcpy(tp_a, tp);
		catch_flag = atoi(tp_a);
	}
	tp = strtok(NULL, "#"); //ball のX座標
	if(tp != NULL){
		strcpy(tp_a, tp);
		ball_distance_0 = atof(tp_a);
	}
	tp = strtok(NULL, "#"); //ballのY座標
	if(tp != NULL){
		strcpy(tp_a, tp);
		ball_theta_1 = atof(tp_a);
	}
	tp = strtok(NULL, "#"); //confidence_sum
	if(tp != NULL){
		strcpy(tp_a, tp);
		confidence_sum_1 = atof(tp_a);
	}
	tp = strtok(NULL, "#"); //SD
	if(tp != NULL){
		strcpy(tp_a, tp);
		SD_1 = atof(tp_a);
	}
	tp = strtok(NULL, "#"); //circularity
	if(tp != NULL){
		strcpy(tp_a, tp);
		circularity_1 = atof(tp_a);
	}

	// 1回転以上した場合はそれを修正
	while(robot_deg > 360) robot_deg -= 360;
	while(robot_deg <-360) robot_deg += 360;

	// [-180, 180]の範囲に収まるように修正
	if(robot_deg > 180) robot_deg -= 360;
	if(robot_deg <-180) robot_deg += 360;

	strcpy(recv_msg, "");
	fflush(stdin);

	return num_of_recv;
}

///

void connectWithMCClass::connectLoop()
{
	// while(1){std::this_thread::sleep_for(std::chrono::seconds(3)); sendData();}
	// return;

	// 受信用ソケット
	/*
	ser_Deg = CreateTCPServerSocket(PORT_Deg);
	PRINT("ID_Pio of server socket (create) is %d", ser_Deg);
	PRINT("waiting to be connected from any client(Pioneer)...\n");
	acc_Deg = AcceptTCPClient(ser_Deg);
	PRINT("ID_Pio of server socket (accept) is %d", acc_Deg);
	*/

			PRINT("ここまできてるのかな");
			// 送信用ソケット(1/2)
			ser_Pio = CreateTCPServerSocket(PORT_Cli_Pio);


	//std::cout << "ID_Pio of server socket (create) is " << ser_Pio << std::endl;
	// 何度でも再接続を試みる
	while(thread_continue_flag)
	{

		struct timeval tv;
		tv.tv_sec = 30;  /* 30 Secs Timeout */
		tv.tv_usec = 0;  // Not init'ing this can cause strange errors
		setsockopt(ser_Pio, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv,sizeof(struct timeval));

//PRINT("aaaaa\n");
		// 送信用ソケット(2/2)
		PRINT("The thread \"SendToOnboard\" is waiting to be connected...");
		acc_Pio = AcceptTCPClient(ser_Pio);
		PRINT("The thread \"SendToOnboard\" is connected.");
		//std::cout << "ID_Pio of server socket (accept) is " << acc_Pio << std::endl;
		// if(!data.pioneer_activate) std::cout<< "waiting command for activating (please press 's' to activate pioneer)"<<std::endl;

		// 送受信
		while(thread_continue_flag)
		{
			// フラグが立ったら
			// while(!*is_new);
			// *is_new = false;
			// 一定時間ごとに実行
			std::this_thread::sleep_for(std::chrono::milliseconds(100));




// fd_set rfds;
// struct timeval tv;
//
// FD_ZERO(&rfds);
// FD_SET(ser_Pio, &rfds);
//
//
// tv.tv_sec = 5;
// tv.tv_usec = 0;
// int judge;

// judge = select(ser_Pio+1, &rfds, NULL, NULL, &tv);


			// 送信->受信
			if(sendData() == -1) break;





		  // fd_set rfds;
		  // struct timeval tv;
      //
		  // FD_ZERO(&rfds);
		  // FD_SET(ser_Pio, &rfds);
      //
		  // tv.tv_sec = 0;
		  // tv.tv_usec = 1000 * 1000;
      //
		  // int judge;
			// judge = select(ser_Pio+1, &rfds, NULL, NULL, &tv);
      //
			// // -：シグナル受信
			// // 0：タイムアウト
			// // else：エラー？
      //
			// PRINT("setTimeOut: %d", judge);


			if(receiveData() == -1) break;
		}
		PRINT("おわったよ〜");

		closesocket(ser_Pio);
		PRINT("閉じれたよ〜");
	}

	// 該当する関数がUnix向けには見当たらない
	#if defined(_WIN32) || defined(_WIN64)
	closesocket(ser_Pio);
	#endif

	PRINT("the connect with MC thread finished its task.");
}

///

void connectWithMCClass::connectLoopThreadBegin(bool *is_new)
{
	thread_1 = new std::thread(&connectWithMCClass::connectLoop, this);
	this->is_new = is_new;
}

void connectWithMCClass::connectLoopThreadFinish()
{
	if(thread_1==nullptr)
	{
		PRINT("the thread instance had not activated.");
	}
	else
	{
		thread_continue_flag = false;
		thread_1->join();
		thread_1=nullptr;
		PRINT("the thread instance has successfully removed.");
	}
}

connectWithMCClass::~connectWithMCClass()
{
	connectLoopThreadFinish();
}

void connectWithMCClass::dataInput(DataClass *data_in)
{
	// PRINT("詳細なデバッグ：%d  %lf  %lf  %lf  %lf  %lf  %lf", catch_flag, ball_distance_0, ball_theta_1, confidence_sum_1, SD_1, circularity_1, robot_deg);

	data_in->robot.body_deg = robot_deg;

	data_in->ball_st.catch_flag = catch_flag;
	data_in->ball_st.ball_distance[0] = ball_distance_0;
	data_in->ball_st.ball_theta[0] = ball_theta_1;
	data_in->ball_st.confidence_sum[0] = confidence_sum_1;
	data_in->ball_st.SD[0] = SD_1;
	data_in->ball_st.circularity[0] = circularity_1;
}

void connectWithMCClass::dataOutput(DataClass *data_in)
{
	pioneer_activate = data_in->pioneer_activate;
	command.head_flag = data_in->command.head_flag;
	sub_goal = data_in->sub_goal;
	ball_st_hand_flag = data_in->ball_st.hand_flag;
	sound_gaze_angle = 0, hark_sd = 0;  // とりあえず
	robot = data_in->robot.posi;
	command = data_in->command;
	robot_move_or_halt = data_in->robot_move_or_halt;
}
