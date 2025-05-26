#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <thread>
#include <vector>
#include <string.h>
#include "opengl.hpp"

#if defined(_WIN32) || defined(_WIN64)
	#include <Windows.h>
#endif

// クラス
#include "class.hpp"

// 非クラス
#include "00_read_config.hpp"
#include "01_zps.hpp"
#include "02_gl.hpp"
#include "05_connect_mc.hpp"
#include "nk_Connect_Unity.hpp"
#include <i_connect_environment_map.hpp>
// #include "06_connect_hark.hpp"
#include "tk30_ActivateBehavior.hpp"
#include "10_parameter.hpp"
// #include "99_print.hpp"
#include "tk0_MainThread.hpp"

// 大域変数
#include "gv_declaration_id.hpp"
#include "gv_declaration_basic.hpp"
#include "tk_data_declaration.hpp"

#include <random>  // 乱数

void input_weight()	//活動度合い[60][30]を読み込み
{

	std::ifstream ifs("weight1.txt");
	std::string str;
	int i=0,j=0;

	if(ifs.fail()) {
		std::cerr << "File do not exist.\n";
		exit(0);
	}

	 while(getline(ifs, str))
	    {
	        std::string tmp = "";
	        std::istringstream stream(str);

	        // 区切り文字がなくなるまで文字を区切っていく
	        while (getline(stream, tmp, ','))
	        {
	            // 区切られた文字がtmpに入る
	            data.weight[i][j] = atof(tmp.c_str());
	            j++;
	        }

	        j = 0;
	        i++;  // 次の人の配列に移る
	    }
}

int main(int argc, char** argv)
{

	// ランダムなオーナー/ストレンジャー選択（実験用）
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<int> score(0, 1);
	data.OSinverse = score(mt);
	PRINT("OSinverse: %d", data.OSinverse);
	data.OSinverse = 1;

	// 実行日時を取得
	time_t now = time(NULL);
	struct tm *pnow = localtime(&now);
	sprintf(data.start_time, "%04d%02d%02d_%02d%02d%02d", pnow->tm_year + 1900, pnow->tm_mon + 1, pnow->tm_mday,
	pnow->tm_hour, pnow->tm_min, pnow->tm_sec);
	std::cout << "current time: " << data.start_time << std::endl;

	// デバッグファイルの名前指定
	sprintf(data.dat_file_name_parameters, "./dat/data_%s.txt", data.start_time);
	sprintf(data.dat_file_name_3d, "./dat/3d_data_%s.txt", data.start_time);
	sprintf(data.dat_file_name_gnuplot, "./dat/gnuplot_%s.txt", data.start_time);

	// 実行ファイルのディレクトリを取得
	#if defined(_WIN32) || defined(_WIN64)
		WCHAR path_w[255];
		char path[255];
		char drive[255], dir[255], fname[255], ext[255];

		// 実行ファイルの完全パスを取得
		GetModuleFileName(NULL, path_w, sizeof(path));
		//wprintf(L"%s\n", path_w);

		// wchar から char へ変換
		setlocale(LC_CTYPE, "jpn");
		wcstombs(path, path_w, 255);
		//printf("%s\n", path);

		// パスをいったん要素ごとに分解し，ディレクトリになるように結合し、最後の'/'は削除
		_splitpath(path, drive, dir, fname, ext);
		sprintf(config.cwd, "%s%s", drive, dir);
		config.cwd[strlen(config.cwd) - 1] = '\0';
		//std::cout << config.cwd << std::endl;
	#else
		char *p;

		// 実行ファイルの絶対パスを取得
		realpath(argv[0] , config_data.cwd);
		std::cout << config_data.cwd << std::endl;

		// ファイル名を削除しディレクトリにする
		p = strrchr(config_data.cwd, '/');
		config_data.cwd[(int)(p-config_data.cwd)] = '\0';
		std::cout << config_data.cwd << std::endl;
	#endif

	// 設定値読み込み
	if(ReadConfigXML(config_data.cwd) != 0)
	{
		std::cout << "設定ファイルの読み込みエラー" << std::endl;
		return -1;
	}

	PRINT("exp_condition: %d", exp_condition);

	human_1.setPosition(4650, 3600);
	human_2.setPosition(2000, 3500);

	// スレッド立ち上げ

	// メイン処理
	{
		std::thread tmp(MainThread);
		threads.push_back(std::move(tmp));
	}
	{
		// std::thread tmp(StressThread);
		// threads.push_back(std::move(tmp));
	}

	// デバッグ
	Debug.ThreadBegin(&data.loop_num, &data.plot_type);  // 内部でスレッド立ち上げ


	// 通信：ZPS
	if(poR == 1 || poO == 1 || poS == 1 || poF == 1)
	{
		//connectWithZPS.receiveLoopThreadBegin(&data.zps_robot_new);  // 内部でスレッド立ち上げ
		connectWithUnity.receiveLoopThreadBegin(&data.zps_robot_new); //内部でスレッド立ち上げ
	}

	// 通信：実機
	if(ROBOT == 1)
	{
		connectWithMC.connectLoopThreadBegin(&data.is_new);  // 内部でスレッド立ち上げ
	}
	else
	{
		RobotSimulator.moveLoopThreadBegin();  // 内部でスレッド立ち上げ
	}

	if(map ==1)
	{
		connectWithEM.connectLoopThreadBegin(&data.is_new);  // 内部でスレッド立ち上げ
	}

	// HARK
	// if(HARK_connect == 1)
	// {
	// 	std::thread tmp(RecieveFromHARK);
	// 	threads.push_back(std::move(tmp));
	// }

	// 描画
	handleOpenGL.begin(argc, argv);


	return 0;
}
