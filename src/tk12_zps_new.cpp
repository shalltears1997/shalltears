#include <cstring>
#include <iostream>


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

#include "01_zps.hpp"

// ZPSの修正用（いまは不要かも）  // tk170111
#if defined(_WIN32) || defined(_WIN64)
	//過去のWindows用として
	#include <mmsystem.h>
#else
	//過去のlinux用として
	#include <chrono>
#endif

double connectWithZPSClass::dist(double x, double y)
{
	return sqrt(pow((x - y), 2));
}

double connectWithZPSClass::siz(double x, double y)
{
	return sqrt(pow(x, 2) + pow(y, 2));
}

int connectWithZPSClass::receive()
{
	static int n = 0;  // tk170111
	char recv_msg[127] = {""}; // 受信データを格納
	int num_of_recv = 0; //受信メッセージ長
	ZPSData tmp_zps; //一時的な受信データ格納
	static int RTagID, R1TagID, R2TagID, OTagID,STagID,FTagID;
	static float R1posX,R1posY,R1posZ;
	static float R2posX,R2posY,R2posZ;
	static float RposX,RposY,RposZ,Rdeg;

	static bool L_side = false;
	static bool R_side = false;
	static bool singletag = false;
	char* tp;
	char tp_a[64] = "";
	char recv_msg_cp[256] = "";

	// ZPSの修正用（いまは不要かも）  // tk170111
#if defined(_WIN32) || defined(_WIN64)
	//#include <windows.h>			// おそらく不要になったやつ
	//#pragma comment(lib, "winmm.lib")	// おそらく不要になったやつ
	DWORD lastTGT;
	DWORD initialTGT = timeGetTime();
#else
	std::chrono::system_clock::time_point now_chrono;
	std::chrono::system_clock::time_point last_chrono = std::chrono::system_clock::now();
#endif
	double timeInterval;

	// #ifdef NEW_MC
	// 空データを送信
	send(cri, "gh", 2, 0);  // tk170111、内容に意味はない（送ることに意味がある）
	// #endif


	//データを受信
	if(( num_of_recv = recv(cri, recv_msg, NUM_OF_RECV_DATA, 0 )) == -1 )
	{
  	perror("Recieve data from ZPS");
  	return -1;
	}
	//データの表示
	// PRINT("recv from ZPS (%d): %s ", num_of_recv, recv_msg);

	strcpy(recv_msg_cp, recv_msg);

	tp = strtok(recv_msg, ","); //1つ目：タグ番号
	if(tp != NULL){
		strcpy(tp_a, tp);
		tmp_zps.tag = atoi(tp_a);
	}
	tp = strtok(NULL, ","); //２つ目：タイプ
	if(tp != NULL){
		strcpy(tp_a, tp);
		tmp_zps.type = tp_a[0];
	}
	tp = strtok(NULL, ","); //３つ目：X座標
	if(tp != NULL){
		strcpy(tp_a, tp);
		tmp_zps.pos[0] = atof(tp_a);
	}
	tp = strtok(NULL, ","); //４つ目：Y座標
	if(tp != NULL){
		strcpy(tp_a, tp);
		tmp_zps.pos[1] = atof(tp_a);
	}
	tp = strtok(NULL, ","); //３つ目：Z座標
	if(tp != NULL){
		strcpy(tp_a, tp);
		tmp_zps.pos[2] = atof(tp_a);
	}

	//対応するデータを更新
	if( tmp_zps.type == 'M' )  //1タグ方式：移動ロボットのタグ
	{
		*zps_robot_new = true;

		singletag = true;
		if(tmp_zps.pos[0] == 0.0 && tmp_zps.pos[1] == 0.0)
		{
			PRINT("pioneer position error\n");
		}
		else
		{
			// pioneer.setPosition((double)tmp_zps.pos[1] - ZPS_Yo, ZPS_Xo - (double)tmp_zps.pos[0], 0);
			robot_c = {(double)tmp_zps.pos[1]-ZPS_Yo, ZPS_Xo-(double)tmp_zps.pos[0]};
		}


		#if defined(_WIN32) || defined(_WIN64)
			// tk170111
			timeInterval = (double)(timeGetTime() - lastTGT)/CLOCKS_PER_SEC;
			//printf("timeInterval:M: %7.3lf  %7.3lf | %7.3lf  %7.3lf\n", (double)(timeGetTime() - initialTGT)/CLOCKS_PER_SEC, timeInterval, (double)tmp_zps.pos[1] - ZPS_Yo, ZPS_Xo - (double)tmp_zps.pos[0]);
			lastTGT = timeGetTime();
		#else
			now_chrono = std::chrono::system_clock::now();
			//timeInterval = std::chrono::duration_cast<std::chrono::milliseconds>(now_chrono - last_chrono).count();
			timeInterval = std::chrono::duration_cast<std::chrono::seconds>(now_chrono - last_chrono).count();
			last_chrono = now_chrono;
		#endif

	}
	else if( tmp_zps.type == 'L' ){  //ロボットの左側のタグ  // 使用なしのはず（171127塚田）
		if(tmp_zps.pos[0] == 0.0 && tmp_zps.pos[1] == 0.0){
			L_side = false;
		}
		else{
			R1TagID = tmp_zps.tag;
			R1posX = (double)tmp_zps.pos[1] - ZPS_Yo;
			R1posY = ZPS_Xo - (double)tmp_zps.pos[0];
			R1posZ = tmp_zps.pos[2];
			L_side = true;
		}
	}
	else if( tmp_zps.type == 'R' ){  //ロボットの右側のタグ  // 使用なしのはず（171127塚田）
		if(tmp_zps.pos[0] == 0.0 && tmp_zps.pos[1] == 0.0){
			R_side = false;
		}
		else{
			R2TagID = tmp_zps.tag;
			R2posX = (double)tmp_zps.pos[1] - ZPS_Yo;
			R2posY = ZPS_Xo - (double)tmp_zps.pos[0];
			R2posZ = tmp_zps.pos[2];
			R_side = true;
		}
	}
	else if( tmp_zps.type == 'O' )
	{  //Ownerのタグ
		if(tmp_zps.pos[0] == 0.0 && tmp_zps.pos[1] == 0.0)
		{  //Owner＆StrangerのZPStagが反応しない際，シミュレータに表示させないようにする
			// human_0.setPosition(-3300.0,-1000.0,1000.0);
			// エラー
		}
		else
		{
			OTagID = tmp_zps.tag;
			// human_0.setPosition((double)tmp_zps.pos[1]-ZPS_Yo, ZPS_Xo-(double)tmp_zps.pos[0], (double)tmp_zps.pos[2]);
			human_c[0] = {(double)tmp_zps.pos[1]-ZPS_Yo, ZPS_Xo-(double)tmp_zps.pos[0]};
			human_height[0] = (double)tmp_zps.pos[2];
		}
		//cout << "OposX:" << OposX << endl;
		//cout << "OposY:" << OposY << endl;
	}
	else if( tmp_zps.type == 'S' )
	{ //Strangerのタグ
		if(tmp_zps.pos[0] == 0.0 && tmp_zps.pos[1] == 0.0)
		{  //Owner＆StrangerのZPStagが反応しない際，シミュレータに表示させないようにする
			// human_1.setPosition(-3300.0,-1000.0,1000.0);
			// エラー
		}
		else
		{
			STagID = tmp_zps.tag;
			// human_1.setPosition((double)tmp_zps.pos[1]-ZPS_Yo, ZPS_Xo-(double)tmp_zps.pos[0], (double)tmp_zps.pos[2]);
			human_c[1] = {(double)tmp_zps.pos[1]-ZPS_Yo, ZPS_Xo-(double)tmp_zps.pos[0]};
			human_height[1] = (double)tmp_zps.pos[2];
		}
		//cout << "OposX:" << OposX << endl;
		//cout << "OposY:" << OposY << endl;
	}
	else if( tmp_zps.type == 'F' )
	{ //Strangerのタグ
		if(tmp_zps.pos[0] == 0.0 && tmp_zps.pos[1] == 0.0)
		{  //Owner＆StrangerのZPStagが反応しない際，シミュレータに表示させないようにする
			// human_2.setPosition(-3300.0,-1000.0,1000.0);
			// エラー
		}
		else
		{
			FTagID = tmp_zps.tag;
			// human_2.setPosition((double)tmp_zps.pos[1] - ZPS_Yo,ZPS_Xo - (double)tmp_zps.pos[0] , (double)tmp_zps.pos[2]);
			human_c[2] = {(double)tmp_zps.pos[1]-ZPS_Yo, ZPS_Xo-(double)tmp_zps.pos[0]};
			human_height[2] = (double)tmp_zps.pos[2];
		}
	}

	static int Rcnt = 0;
	if(poR == 1 && L_side == true && R_side == true)
	{
		RposX = (R1posX + R2posX)/2.0;
		RposY = (R1posY + R2posY)/2.0;
		RposZ = (R1posZ + R2posZ)/2.0;
		Rdeg  = orthoDeg( R2posY-R1posY,R2posX - R1posX);
		// pioneer.setPosition(RposX,RposY,Rdeg);
		// pioneer.Head(targetpose); // tk1701
		PRINT("上をコメントアウトしたけど要検証！pioneer head");//: %lf", pioneer.getDeg());
		Rcnt = 0;
		L_side = false; R_side = false;
	}
	else if(poR == 1 && !singletag &&(L_side == false || R_side == false))
	{
		Rcnt++;
		if(Rcnt > 5){
			PRINT("Error : ");
			if(L_side == false)	PRINT("L-side ");
			if(R_side == false)	PRINT("R-side ");
			PRINT("tag");
		}
	}

	strcpy(recv_msg, "");
	fflush(stdin);

	return num_of_recv; //受信文字数を返す
}


void connectWithZPSClass::receiveLoop()
{
	cri = CreateTCPClientSocket(PORT_Ser, IP_ZPS);
	PRINT("ID of crient socket is %d", cri);

	int out = NUM_OF_RECV_DATA;
	while(out == NUM_OF_RECV_DATA && thread_continue_flag)
	{
		// ZPSクライアントに対して、データの送信->受信 を行う
		out = receive();
	}

	PRINT("the connect with ZPS thread finished its task.");
}


void connectWithZPSClass::receiveLoopThreadBegin(bool *zps_robot_new)
{
	thread_1 = new std::thread(&connectWithZPSClass::receiveLoop, this);
	this->zps_robot_new = zps_robot_new;
}

void connectWithZPSClass::receiveLoopThreadFinish()
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

connectWithZPSClass::~connectWithZPSClass()
{
	receiveLoopThreadFinish();
}

void connectWithZPSClass::dataInput(DataClass *data_in)
{
	data_in->robot.posi = robot_c;
	PRINT("%d", exp_condition);

	if(exp_condition)
	{
		if(data.OSinverse==0)
		{
			data_in->human[0].posi = human_c[0];
			data_in->human[1].posi = human_c[1];
		}
		else
		{
			data_in->human[0].posi = human_c[1];
			data_in->human[1].posi = human_c[0];
		}
	}
	else
	{
		data_in->human[0].posi = human_c[0];
		data_in->human[1].posi = human_c[1];
	}

	data_in->human[2].posi = human_c[2];
	data_in->human[0].height = human_height[0];
	data_in->human[1].height = human_height[1];
	data_in->human[2].height = human_height[2];
}

DATA_xy connectWithZPSClass::dataInput_human(int human_n)
{
	if(exp_condition && data.OSinverse==1)
	{
		if(human_n==0) human_n = 1;
		if(human_n==1) human_n = 0;
	}

	return human_c[human_n];
}
