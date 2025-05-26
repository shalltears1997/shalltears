#pragma once

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <chrono>
#include <thread>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

class DebugClass
{


	// ファイルへの出力
  // std::ofstream log_file(dat_file_name, std::ios::app);

	// gnuplot
	FILE *gid;
	FILE *gnuplot_3d;

	FILE *gnuplot_b;

	FILE *gnuplot_3d_1;
	FILE *gnuplot_3d_2;
	FILE *gnuplot_3d_3;
	FILE *gnuplot_3d_4;

	// データセット
	char send_message[1024] = "";

	void disp_bar(double cogv, double efctv);
	void prepare_printDebug();
	void prepare_logSave();
	void prepare_plotParameters();
	void prepare_plot3d();
	void prepare_plot3dmap();

	void prepare_plotBehavior();


	int *loop_num;
	int *plot_type;



	// スレッド
	std::thread *thread_1;
	bool thread_continue_flag = true;

	// メソッド
	void plotParameters();
	void saveDat_plot3d();
	void plot3d();
	void plot3dmap();

	void plotBehabior();


public:

	DebugClass();
	~DebugClass();

  // スレッド
  void ThreadBegin(int *loop_num, int *plot_type);  // ループ受信のスレッドを開始
  void ThreadFinish();  // ループ受信のスレッドを終了
	void plotLoop();

	// メソッド
	void printDebug(int loop_num, double time_duration, double time_interval, double computing_time);
	void logSave(int loop_num, double time_duration, double time_interval, double computing_time);

};
