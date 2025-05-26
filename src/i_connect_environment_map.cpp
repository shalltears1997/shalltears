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
#include "tk_data_class.hpp"
#include "gv_extern_id.hpp"
#include "gv_extern.hpp"

#include <thread>

#include "22_hark.hpp"

#include <sys/select.h>

unsigned short PORT1 = 45360;
unsigned short PORT2 = 45361;
unsigned short PORT3 = 45362;

const char* IP_MAP = "133.91.73.235";

int connectWithEMClass::sendData()
{

	int num_of_string, return_value;
	const int send_num = 23;
	char send_message[25] = {};

	num_of_string = sprintf(send_message,"%05d,%05d,%05d,%05d",(int)data.human[0].posi.x,(int)data.human[0].posi.y,(int)data.human[1].posi.x,(int)data.human[1].posi.y);

	// データ送信
	send_cnt1 = send(cri1, send_message, num_of_string, 0);

	if(send_cnt1 == send_num) return_value = send_cnt1;
	else return_value = -1;
	return return_value;
}

int connectWithEMClass::receiveWeight()
{
	static float recv_map_factor1[30] = {};
	static float pre_weight[60][30] = {};
	int count = 0;
	static int i=0,j=0;

	while(sizeof(recv_map_factor1)-count>0)
	{
		 count += recv(cri2, (char *)recv_map_factor1 + count, sizeof(recv_map_factor1)-count, 0 );
		//printf("size:%d   count=%d\n",sizeof(recv_map_factor1),count);
	}

	for(j=0;j<gridnum2;j++)
	{
		pre_weight[59-i][j] = recv_map_factor1[j];
	}

		for(j=0;j<gridnum2;j++)
		{
			data.weight[i][j] = pre_weight[i][j];
		}

	i++;

	if(i>59)i=0;


	return 0;

	////////////////////////////////////////////////////////////////////////////
	/*char recv_coordinate[20] ={};
	int move_x = 0,move_y = 0;
	int pID = 0;
	int pinmap = 0;
	const int NUM_OF_RECV = 18;
	char* tp;
	int return_value;

	// データ受信
	recv_cnt1 = recv(cri3, recv_coordinate, NUM_OF_RECV, 0);

	if(recv_cnt1 == -1)
	{

	}else if(recv_cnt1 != NUM_OF_RECV)
	{
		PRINT("だめよ〜だめだめ");

	}else if(recv_cnt1 == NUM_OF_RECV)
	{
		if((tp = strtok(recv_coordinate,",")) != NULL)move_x = atoi(tp);
		if((tp = strtok(NULL,",")) != NULL)move_y = atoi(tp);
		if((tp = strtok(NULL,",")) != NULL)pID = atoi(tp);
		if((tp = strtok(NULL,",")) != NULL)pinmap = atoi(tp);

		PRINT("ID:%d x:%d y:%d inmap:%d\n",pID,-move_x,3000-move_y,pinmap);
	}

	data.object[pID].posi.x = -move_x;
	data.object[pID].posi.y = 3000 - move_y;
	data.object[pID].oID = pID;
	data.object[pID].inmap = pinmap;

	//PRINT("ID:%d x:%d y:%d\n",pID,data.object[pID].posi.x,data.object[pID].posi.y);

	if(recv_cnt1 == NUM_OF_RECV) return_value = recv_cnt1;
	else return_value = -1;
	return return_value;*/
}

int connectWithEMClass::receiveCoordinate()
{
	char recv_coordinate[20] ={};
	int move_x = 0,move_y = 0;
	int pID = 0;
	int pinmap = 0;
	const int NUM_OF_RECV = 18;
	char* tp;
	int return_value;

	// データ受信
	recv_cnt1 = recv(cri3, recv_coordinate, NUM_OF_RECV, 0);

	if(recv_cnt1 == -1)
	{
		PRINT("ああああああああああああああああああああああああああああああああああああ");
	}else if(recv_cnt1 != NUM_OF_RECV)
	{
		PRINT("だめよ");

	}else if(recv_cnt1 == NUM_OF_RECV)
	{
		if((tp = strtok(recv_coordinate,",")) != NULL)move_x = atoi(tp);
		if((tp = strtok(NULL,",")) != NULL)move_y = atoi(tp);
		if((tp = strtok(NULL,",")) != NULL)pID = atoi(tp);
		if((tp = strtok(NULL,",")) != NULL)pinmap = atoi(tp);

		PRINT("ID:%d x:%d y:%d inmap:%d\n",pID,-move_x,3000-move_y,pinmap);
	}

	data.object[pID].posi.x = -move_x;
	data.object[pID].posi.y = 3000 - move_y;
	data.object[pID].oID = pID;
	data.object[pID].inmap = pinmap;



	//PRINT("ID:%d x:%d y:%d\n",pID,data.object[pID].posi.x,data.object[pID].posi.y);

	if(recv_cnt1 == NUM_OF_RECV) return_value = recv_cnt1;
	else return_value = -1;
	return return_value;

}

void connectWithEMClass::connectLoop()
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

		//	PRINT("ここまできてるのかな");
			// 送信用ソケット(1/2)
			//ser_Pio = CreateTCPServerSocket(PORT_Cli_Pio);

	//std::cout << "ID_Pio of server socket (create) is " << ser_Pio << std::endl;
	// 何度でも再接続を試みる


	//cri1 = CreateTCPClientSocket(PORT1, IP_MAP);
	//cri2 = CreateTCPClientSocket(PORT2, IP_MAP);
	//cri3 = CreateTCPClientSocket(PORT3, IP_MAP);

	while(thread_continue_flag)
	{

		struct timeval tv;
		tv.tv_sec = 3;  /* 30 Secs Timeout */
		tv.tv_usec = 0;  // Not init'ing this can cause strange errors
		//setsockopt(ser_Pio, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv,sizeof(struct timeval));


		cri1 = CreateTCPClientSocket(PORT1, IP_MAP);
		cri2 = CreateTCPClientSocket(PORT2, IP_MAP);
		cri3 = CreateTCPClientSocket(PORT3, IP_MAP);

		// 送信用ソケット(2/2)
		//PRINT("The thread \"SendToOnboard\" is waiting to be connected...");
		//acc_Pio = AcceptTCPClient(ser_Pio);
		//PRINT("The thread \"SendToOnboard\" is connected.");


		//std::cout << "ID_Pio of server socket (accept) is " << acc_Pio << std::endl;
		// if(!data.pioneer_activate) std::cout<< "waiting command for activating (please press 's' to activate pioneer)"<<std::endl;

		// 送受信
		while(thread_continue_flag)
		{
			// フラグが立ったら
			// while(!*is_new);
			// *is_new = false;
			// 一定時間ごとに実行
			//std::this_thread::sleep_for(std::chrono::milliseconds(100));

			PRINT("おわったよ〜");
			// 送信->受信
			if(sendData() == -1)
				{
				PRINT("おわっ");
				break;
				}
			//if(receiveCoordinate() == -1)break;
			if(receiveWeight() == -1 || receiveCoordinate() == -1) break;
		}

		fflush(stdin);

		PRINT("出てきちゃいました");

		//closesocket(cri1);
		//closesocket(cri3);

		//PRINT("閉じれたよ〜");
	}

	// 該当する関数がUnix向けには見当たらない
	#if defined(_WIN32) || defined(_WIN64)
	closesocket(ser_Pio);
	#endif

	PRINT("the connect with MC thread finished its task.");
}

///

void connectWithEMClass::connectLoopThreadBegin(bool *is_new)
{
	thread_1 = new std::thread(&connectWithEMClass::connectLoop, this);
	this->is_new = is_new;
}



void connectWithEMClass::connectLoopThreadFinish()
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

connectWithEMClass::~connectWithEMClass()
{
	connectLoopThreadFinish();
}
