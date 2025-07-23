#include <iostream>
#include <fstream>
#include <cstdlib>
#include <chrono>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

#include "define.hpp"
#include "class.hpp"
#include "gv_extern.hpp"
#include "tk30_ActivateBehavior.hpp"
#include "l_tk-library.hpp"

#include "tk22_SelectBehavior.hpp"
#include "tk32_BasicBehaviors.hpp"
#include "tk51_RobotSimulator.hpp"
#include "tk51_SubGoalGenerator.hpp"

#include "tk_data_class.hpp"
#include "tk_Parameter.hpp"

#include "tk02_Debug.hpp"

void DebugClass::disp_bar(double cogv, double efctv)
{
	int W = 26;

	int We = (int)((double)W / sMAX * efctv);
	int Wc = (int)((double)W / sMAX * cogv) - We;
	int Wo = W - We - Wc;

	printf(" |");
	for (int i=0; i<We; i++) printf("\033[1m#\033[0m");
	for (int i=0; i<Wc; i++) printf("\033[1m=\033[0m");
	for (int i=0; i<Wo; i++) printf("-");
	printf("|\n");
}


void DebugClass::prepare_printDebug()
{
	// エスケープシーケンスのために改行しておく
	for(int i=0; i<ESCAPE_OVER_NUM; i++) printf("\n");
}

void DebugClass::prepare_logSave()
{
	std::ofstream log_file(data.dat_file_name_parameters, std::ios::app);
	sprintf(send_message,
			"#\ttime\t"
			"r_X\tr_Y\tr_deg\t"
			"h0_X\th0_Y\th0_H\th0_deg\t"
			"h1_X\th1_Y\th1_H\th1_deg\t"
			"h0_ATA\th0_MIS\th0_ANX"
			"h1_ATA\th1_MIS\th1_ANX");
	log_file << send_message << std::endl;
	log_file.close();
}


void DebugClass::prepare_plotParameters()
{
	// ウィンドウのサイズ・位置の指定
	char gnuplot1_property[128] = "";
	// sprintf(gnuplot1_property, "gnuplot -geometry %dx%d+%d+%d", (int)(display_w*(1-main_w_ratio)), (int)(display_h*0.5)-15-25, (int)(display_w*main_w_ratio), (int)(display_h*0.5));
	sprintf(gnuplot1_property, "gnuplot -geometry %dx%d+%d+%d -display :0.0",
			(int)(data.field_w * (1-data.sim_ratio_w)),
			(int)(data.field_h * data.h_parameters - data.window_margin_top - 11),
			(int)(data.display_x + data.field_w * data.sim_ratio_w),
			(int)(data.display_y + data.field_h * (data.h_score + data.h_behavior)) );
	// sprintf(gnuplot1_property, "gnuplot -geometry %dx%d+%d+%d", 720, 200, 720, 0);

	// gid = popen("gnuplot -persist", "w");  // プログラムを終了した後もグラフを残す
	gid = popen(gnuplot1_property, "w");  // gnuplot開始
}


void DebugClass::prepare_plotBehavior()
{
	// ウィンドウのサイズ・位置の指定
	char gnuplot3_property[128] = "";
	// sprintf(gnuplot1_property, "gnuplot -geometry %dx%d+%d+%d", (int)(display_w*(1-main_w_ratio)), (int)(display_h*0.5)-15-25, (int)(display_w*main_w_ratio), (int)(display_h*0.5));
	sprintf(gnuplot3_property, "gnuplot -geometry %dx%d+%d+%d -display :0.0",
			(int)(data.field_w * (1-data.sim_ratio_w)),
			(int)(data.field_h * data.h_behavior - data.window_margin_top - 11),
			(int)(data.display_x + data.field_w * data.sim_ratio_w),
			(int)(data.display_y + data.field_h * data.h_score) );
	// sprintf(gnuplot1_property, "gnuplot -geometry %dx%d+%d+%d", 720, 200, 720, 0);

	// gid = popen("gnuplot -persist", "w");  // プログラムを終了した後もグラフを残す
	gnuplot_b = popen(gnuplot3_property, "w");  // gnuplot開始
}


void DebugClass::prepare_plot3d()
{
	// ウィンドウのサイズ・位置の指定
	char gnuplot2_property[128] = "";

	sprintf(gnuplot2_property, "gnuplot -geometry %dx%d+%d+%d -display :0.0 >%s 2>&1",
			(int)(data.field_w*(1-data.sim_ratio_w)),
			(int)(data.field_h * data.h_score - data.window_margin_top - 11),  // - 34
			(int)(data.display_x + data.field_w*data.sim_ratio_w),
			(int)(data.display_y + data.field_h * 0),
			data.dat_file_name_gnuplot );

	// gnuplot_3d = popen("gnuplot -persist", "w");  // プログラムを終了した後もグラフを残す
	gnuplot_3d = popen(gnuplot2_property, "w");  // gnuplot開始
}

void DebugClass::prepare_plot3dmap()
{
	// ウィンドウのサイズ・位置の指定
	char gnuplot2_property[128] = "";

	sprintf(gnuplot2_property, "gnuplot -geometry %dx%d+%d+%d -display :0.0 >%s 2>&1",
			(int)(data.field_w*(1-data.sim_ratio_w)*0.5),
			(int)(data.field_h*data.h_map4x*0.5 - data.window_margin_top - 11),  // - 34
			(int)(data.display_x + data.field_w*data.sim_ratio_w),
			data.display_y,
			data.dat_file_name_gnuplot );
	gnuplot_3d_1 = popen(gnuplot2_property, "w");  // gnuplot開始

	sprintf(gnuplot2_property, "gnuplot -geometry %dx%d+%d+%d -display :0.0 >%s 2>&1",
			(int)(data.field_w*(1-data.sim_ratio_w)*0.5),
			(int)(data.field_h*data.h_map4x*0.5 - data.window_margin_top - 11),  // - 34
			(int)(data.display_x + data.field_w*data.sim_ratio_w + data.field_w*(1-data.sim_ratio_w)*0.5),
			data.display_y,
			data.dat_file_name_gnuplot );
	gnuplot_3d_2 = popen(gnuplot2_property, "w");  // gnuplot開始

	sprintf(gnuplot2_property, "gnuplot -geometry %dx%d+%d+%d -display :0.0 >%s 2>&1",
			(int)(data.field_w*(1-data.sim_ratio_w)*0.5),
			(int)(data.field_h*data.h_map4x*0.5 - data.window_margin_top - 11),  // - 34
			(int)(data.display_x + data.field_w*data.sim_ratio_w),
			(int)(data.display_y + data.field_h*data.h_map4x*0.5),
			data.dat_file_name_gnuplot );
	gnuplot_3d_3 = popen(gnuplot2_property, "w");  // gnuplot開始

	sprintf(gnuplot2_property, "gnuplot -geometry %dx%d+%d+%d -display :0.0 >%s 2>&1",
			(int)(data.field_w*(1-data.sim_ratio_w)*0.5),
			(int)(data.field_h*data.h_map4x*0.5 - data.window_margin_top - 11),  // - 34
			(int)(data.display_x + data.field_w*data.sim_ratio_w + data.field_w*(1-data.sim_ratio_w)*0.5),
			(int)(data.display_y + data.field_h*data.h_map4x*0.5),
			data.dat_file_name_gnuplot );
	gnuplot_3d_4 = popen(gnuplot2_property, "w");  // gnuplot開始
}


void DebugClass::printDebug(int loop_num, double time_duration, double time_interval, double computing_time)
{
	// ログの上にカーソルを置く
	// 8行分上にカーソルを置き、8行分(下にある)行を削除する
	// std::cout << "\033[19A\033[19M" << std::flush;
	printf("\033[%dA\033[%dM", ESCAPE_OVER_NUM);


	// 行動名
	char behavior_name[32];
	if     (data.motion_id == GO_TO_SITTING_OWNER) sprintf(behavior_name, "GO_TO_SITTING_OWNER"); // 01  00
	else if(data.motion_id == PLAY_USING_BALL)     sprintf(behavior_name, "PLAY_USING_BALL");     // 02  01
	else if(data.motion_id == EXPLORE)             sprintf(behavior_name, "EXPLORE");             // 21  02
	else if(data.motion_id == MISSING)             sprintf(behavior_name, "MISSING");             // 22  03
	else if(data.motion_id == GO_TO_DOOR)          sprintf(behavior_name, "GO_TO_DOOR");          // 23  04
	else if(data.motion_id == GO_TO_OWNER)         sprintf(behavior_name, "GO_TO_OWNER");         // 24  05
	else if(data.motion_id == EXPLORE_STRANGER)    sprintf(behavior_name, "EXPLORE_STRANGER");    // 25  06
	else if(data.motion_id == ATTENTION_GETTING)   sprintf(behavior_name, "ATTENTION_GETTING");   // 31  07
	else if(data.motion_id == SHOWING_DIRECTION)   sprintf(behavior_name, "SHOWING_DIRECTION");   // 32  08
	else if(data.motion_id == GAZE_ALTERNATION)   sprintf(behavior_name, "GAZE_ALTERNATION");     // 33  09
	else if(data.motion_id == RANDOM)   					 sprintf(behavior_name, "RANDOM"); 					    // 99
	else                                           sprintf(behavior_name, "Unknown; ID=%d.", data.motion_id);


	printf("\n");
	// printf("SYSTEM          LOOP     TIME                                                    \033[0m\n");
	printf("loop: %7d, duration: %7.1lfs, interval: %5.1lfms, cpu_time: %5.1lfms, s?: %d, h?: %d\n", loop_num, time_duration, time_interval, computing_time, GLData.data.pioneer_activate, data.home);

	printf("\033[7m");
	printf("ROBOT         X        Y      DEG cautious  curious   depend  Behavior                         \033[0m\n");
	printf("ROBOT1  %7.1lf  %7.1lf  %7.1lf  %7.1lf  %7.1lf  %7.1lf  %s\n",
			data.robot.posi.x, data.robot.posi.y, data.robot.body_deg,
			1.1, 2.2, 3.3, behavior_name);

							// 仮措置
							PositionClass own, str;
							own.setPos(data.human[0].posi);
							str.setPos(data.human[1].posi);

	// if(exp_condition)
	// {
	// 	if(data.OSinverse==0)
	// 	{
	// 		printf("\033[7m");
	// 		printf("HUMAN         X        Y        D      DEG   HEIGHT     sepa      ata                          \033[0m\n");
	// 		printf("HUMAN0  %7.1lf  %7.1lf  %7.1lf  %7.1lf  %7.1lf  %7.1lf  %7.1lf\n",
	// 				data.human[0].posi.x, data.human[0].posi.y, calDistance(own.pos(), data.robot.posi), data.human[0].deg, data.human[0].height,
	// 				data.human_non_exist_duration[0], data_pre.pa_human[0].ata);
	// 		printf("HUMAN1  %7.1lf  %7.1lf  %7.1lf  %7.1lf  %7.1lf  %7.1lf  %7.1lf\n",
	// 				data.human[1].posi.x, data.human[1].posi.y, calDistance(str.pos(), data.robot.posi), data.human[1].deg, data.human[1].height,
	// 				data.human_non_exist_duration[1], data_pre.pa_human[1].ata);
	// 	}
	// 	else
	// 	{
	// 		printf("\033[7m");
	// 		printf("HUMAN         X        Y        D      DEG   HEIGHT     sepa      ata                          \033[0m\n");
	// 		printf("HUMAN0  %7.1lf  %7.1lf  %7.1lf  %7.1lf  %7.1lf  %7.1lf\n",
	// 				data.human[1].posi.x, data.human[1].posi.y, calDistance(own.pos(), data.robot.posi), data.human[1].deg, data.human[1].height,
	// 				data.human_non_exist_duration[1]);
	// 		printf("HUMAN1  %7.1lf  %7.1lf  %7.1lf  %7.1lf  %7.1lf  %7.1lf\n",
	// 				data.human[0].posi.x, data.human[0].posi.y, calDistance(str.pos(), data.robot.posi), data.human[0].deg, data.human[0].height,
	// 				data.human_non_exist_duration[0]);
	// 	}
	// }
	// else
	{
		printf("\033[7m");
		printf("HUMAN         X        Y        D      DEG   HEIGHT     sepa      ata                          \033[0m\n");
		printf("HUMAN0  %7.1lf  %7.1lf  %7.1lf  %7.1lf  %7.1lf  %7.1lf  %7.1lf\n",
				data.human[0].posi.x, data.human[0].posi.y, calDistance(own.pos(), data.robot.posi), data.human[0].deg, data.human[0].height,
				data.human_non_exist_duration[0], data_pre.pa_human[0].ata);
		printf("HUMAN1  %7.1lf  %7.1lf  %7.1lf  %7.1lf  %7.1lf  %7.1lf  %7.1lf\n",
				data.human[1].posi.x, data.human[1].posi.y, calDistance(str.pos(), data.robot.posi), data.human[1].deg, data.human[1].height,
				data.human_non_exist_duration[1], data_pre.pa_human[1].ata);
	}


	printf("\033[7m");
	printf("STRESS  DEPEND     PLUS    MINUS     DIFF    VALUE    \033[1mEFCTV      F                             \033[0m\n");

	double d;
	int esc;

	// d = data.pa_human[0].danx;
	// if(d>0) esc = 31;
	// else if(d<0) esc = 34;
	// else esc = 0;
	// printf("%-7.7s  %5.3lf  +%5.3lf  -%5.3lf  \033[%dm%+06.3lf  %7.2lf  \033[1m%7.2lf\033[0m  %5.3lf",
	// "H0  ANX", data.K_anx, data.pa_human[0].danxP, abs(data.pa_human[0].danxM), esc, data.pa_human[0].danx, data.pa_human[0].anx_c, data.pa_human[0].anx,
	// exp(-data.human_non_exist_duration[0]/(data.TimeConstant1*data.K_anx)) * exp(-data.human_non_exist_duration[0]/(data.TimeConstant2*data.K_anx)));
	// disp_bar(data.pa_human[0].anx_c, data.pa_human[0].anx);
  //
	// d = data.pa_human[0].dexp;
	// if(d>0) esc = 31;
	// else if(d<0) esc = 34;
	// else esc = 0;
	// printf("%-7.7s  %5.3lf  +%5.3lf  -%5.3lf  \033[%dm%+06.3lf  %7.2lf  \033[1m%7.2lf\033[0m  %5.3lf",
	// "    EXP", data.K_exp, data.pa_human[0].dexpP, abs(data.pa_human[0].dexpM), esc, data.pa_human[0].dexp, data.pa_human[0].exp_c, data.pa_human[0].exp,
	// exp(-data.human_non_exist_duration[0]/(data.TimeConstant2*data.K_exp)));
	// disp_bar(data.pa_human[0].exp_c, data.pa_human[0].exp);

	d = data.pa_human[0].dmis;
	if(d>0) esc = 31;
	else if(d<0) esc = 34;
	else esc = 0;
	printf("%-7.7s  %5.3lf  +%06.3lf  -%06.3lf  \033[%dm%+07.3lf  %7.2lf  \033[1m%7.2lf\033[0m  %5.3lf",
	"O   MIS", data.K_mis, data.pa_human[0].dmisP, abs(data.pa_human[0].dmisM), esc, data.pa_human[0].dmis, data.pa_human[0].mis_c, data.pa_human[0].mis,
	exp(-data.human_non_exist_duration[0]/(data.TimeConstant1*data.K_mis)));
	disp_bar(data.pa_human[0].mis_c, data.pa_human[0].mis);

	// H1

	d = data.pa_human[1].danx;
	if(d>0) esc = 31;
	else if(d<0) esc = 34;
	else esc = 0;
	printf("%-7.7s  %5.3lf  +%06.3lf  -%06.3lf  \033[%dm%+07.3lf  %7.2lf  \033[1m%7.2lf\033[0m  %5.3lf",
	"S   ANX", data.K_anx, data.pa_human[1].danxP, abs(data.pa_human[0].danxM), esc, data.pa_human[1].danx, data.pa_human[1].anx_c, data.pa_human[1].anx,
	exp(-data.human_non_exist_duration[0]/(data.TimeConstant1*data.K_anx)) * exp(-data.human_non_exist_duration[1]/(data.TimeConstant2*data.K_anx)));
	disp_bar(data.pa_human[1].anx_c, data.pa_human[1].anx);

	d = data.pa_human[1].dexp;
	if(d>0) esc = 31;
	else if(d<0) esc = 34;
	else esc = 0;
	printf("%-7.7s  %5.3lf  +%06.3lf  -%06.3lf  \033[%dm%+07.3lf  %7.2lf  \033[1m%7.2lf\033[0m  %5.3lf",
	"    EXP", data.K_exp, data.pa_human[1].dexpP, abs(data.pa_human[1].dexpM), esc, data.pa_human[1].dexp, data.pa_human[1].exp_c, data.pa_human[1].exp,
	exp(-data.human_non_exist_duration[1]/(data.TimeConstant2*data.K_anx)));
	disp_bar(data.pa_human[1].exp_c, data.pa_human[1].exp);

	// d = data.pa_human[1].dmis;
	// if(d>0) esc = 31;
	// else if(d<0) esc = 34;
	// else esc = 0;
	// printf("%-7.7s  %5.3lf  +%5.3lf  -%5.3lf  \033[%dm%+06.3lf  %7.2lf  \033[1m%7.2lf\033[0m  %5.3lf",
	// "    MIS", data.K_mis, data.pa_human[1].dmisP, abs(data.pa_human[1].dmisM), esc, data.pa_human[1].dmis, data.pa_human[1].mis_c, data.pa_human[1].mis,
	// exp(-data.human_non_exist_duration[1]/(data.TimeConstant1*data.K_mis)));
	// disp_bar(data.pa_human[1].mis_c, data.pa_human[1].mis);

	// P

	d = data.explore-data_pre.explore;
	if(d>0) esc = 31;
	else if(d<0) esc = 34;
	else esc = 0;
	printf("%-7.7s  %5.3lf  +%06.3lf  -%06.3lf  \033[%dm%+07.3lf  %7.2lf  \033[1m%7.2lf\033[0m  %5.3lf",
	"iSP EXP", 1.0, 0.0, 0.0, esc, d, 0.0, data.explore, 1.0);
	disp_bar(data.explore, data.explore);

	// ファイルへの出力
	// char data.dat_file_name_parameters[128] = "";
	// sprintf(data.dat_file_name_parameters, "./dat/data_%s.txt", data.start_time);
	// std::ofstream log_file(data.dat_file_name_parameters, std::ios::app);
	// データセット
	// char send_message[1024] = "";
}

void DebugClass::logSave(int loop_num, double time_duration, double time_interval, double computing_time)
{
	std::ofstream log_file(data.dat_file_name_parameters, std::ios::app);

	int tmp_id = data.motion_id;
	if(tmp_id > 10) tmp_id-=18;

	tmp_id--;
	if(tmp_id>1) tmp_id--;

  sprintf(send_message,
		"%5d\t%7.2lf\t" //"%7.1lf\t"
		"%7.1lf\t%7.1lf\t%7.1lf\t"
		"%7.1lf\t%7.1lf\t%7.1lf\t%7.1lf\t"
		"%7.1lf\t%7.1lf\t%7.1lf\t%7.1lf\t"
		"%7.1lf\t%7.1lf\t%7.1lf\t"
		"%7.1lf\t%7.1lf\t%7.1lf\t"
		"%7.1lf\t"
		"%3d\t"
		"%7.1lf\t%7.1lf\t"
		"%7.1lf\t%7.1lf\t%7.1lf\t"
		"%d\t"
		"%7.1lf\t"
		"%3d\t"
		"%7.1lf\t"
		"%7.1lf\t",
		loop_num, time_duration,// computing_time,
		data.robot.posi.x, data.robot.posi.y, data.robot.body_deg,
		data.human[0].posi.x, data.human[0].posi.y, data.human[0].deg, data.human[0].height,
		data.human[1].posi.x, data.human[1].posi.y, data.human[1].deg, data.human[1].height,
		data.pa_human[0].ata, data.pa_human[0].mis, data.pa_human[0].anx,
		data.pa_human[1].ata, data.pa_human[1].exp, data.pa_human[1].anx,
		data.explore,
		tmp_id,
		data.tmp_goal.x, data.tmp_goal.y,
		data.pa_human[0].mis_c, data.pa_human[1].exp_c, data.pa_human[1].anx_c,
		data.OSinverse,
		velocity,
		data.motion_id,
		print_delta2,		
		sound_flag
	);



  // sprintf(send_message,
  // 		"%5d\t%7.2lf\t"
  // 		"%7.1lf\t%7.1lf\t%7.1lf\t"
  // 		"%7.1lf\t%7.1lf\t%7.1lf\t%7.1lf\t%7.1lf\t"
  // 		"%7.1lf\t%7.1lf\t%7.1lf\t%7.1lf\t%7.1lf\t"
  //
	// 		"%7.1lf\t%7.1lf\t%7.1lf\t%7.1lf\t%7.1lf\t%7.1lf"
	// 		"%7.1lf\t%7.1lf\t%7.1lf\t%7.1lf\t%7.1lf\t%7.1lf"
  // 		"%7.1lf\t%7.1lf\t%7.1lf\t%7.1lf\t%7.1lf\t%7.1lf"
  //
  // 		"%7.1lf\t%7.1lf\t%7.1lf"
  // 		"%7.1lf",
  // 		loop_num, time_duration,
  // 		data.robot.posi.x, data.robot.posi.y, data.robot.body_deg,
  // 		data.human[0].posi.x, data.human[0].posi.y, data.human[0].deg, data.human[0].height, data.pa_human[0].ata,
  // 		data.human[1].posi.x, data.human[1].posi.y, data.human[1].deg, data.human[1].height, data.pa_human[1].ata,
  //
  //
	// 		// 増分、減分、差分、認知値、実効値、忘却率
	// 		data.pa_human[0].dmisP, data.pa_human[0].dmisM, data.pa_human[0].dmis, data.pa_human[0].mis_c, data.pa_human[0].mis,
	// 		exp(-data.human_non_exist_duration[0]/(data.TimeConstant1*data.K_mis)),
  //
  //
  // 		data.pa_human[0].mis, data.pa_human[0].anx,
  // 		data.pa_human[1].exp, data.pa_human[1].anx,
  // 		data.explore);

  // 書き込み
  log_file << send_message << std::endl;
	log_file.close();
}


// グラフ表示：gnuplot
void DebugClass::plotParameters()
{
	// 仮処置
	int loop_num = *this->loop_num;

  // if(1&&loop_num % 3 == 0)  // 100msごとに描画するとチラついてしまう
  {
  	// 準備
  	int plot_width = 1000, plot_margin = 0;
  	fprintf(gid, "set multiplot layout 3,1 rowsfirst downwards\n");  // 並べて表示ここから
  	// fprintf(gid, "set title font \"Helvetica, 13\"\n");  // タイトル
  	fprintf(gid, "set key left top\n");  // 凡例の位置
  	fprintf(gid, "set yrange [0:%d]\n", sMAX);  // y軸の範囲
  	fprintf(gid, "set xrange [%d:%d]\n", loop_num + plot_margin - plot_width, plot_margin + loop_num);  // x軸の範囲

  	// プロット：なつき度
  	// fprintf(gid, "set title \"ATTACHMENT\"\n");
  	// fprintf(gid, "plot \"%s\" using 1:14 every ::%d::%d with lines title \"Owner\" lw 5 lc \"#FF0080\",\\\n", data.dat_file_name_parameters, loop_num - plot_width, loop_num);
  	// fprintf(gid, "\"%s\" using 1:17 every ::%d::%d with lines title \"Stranger\" lw 5 lc \"#6666FF\"\n", data.dat_file_name_parameters, loop_num - plot_width, loop_num);

  	// プロット：オーナー
  	// fprintf(gid, "set title \"inner states for OWNER\"\n");
  	fprintf(gid, "plot \"%s\" using 1:15 every ::%d::%d with lines title \"Miss\" lw 5 lc \"#FF0080\"\n", data.dat_file_name_parameters, loop_num - plot_width, loop_num);

  	// プロット：ストレンジャー
  	// fprintf(gid, "set title \"inner states for STRANGER\"\n");
  	fprintf(gid, "plot \"%s\" using 1:18 every ::%d::%d with lines title \"Miss\" lw 5 lc \"#6666FF\",\\\n", data.dat_file_name_parameters, loop_num - plot_width, loop_num);
  	fprintf(gid, "\"%s\" using 1:19 every ::%d::%d with lines title \"Anxiety\" lw 5 lc \"#66CCFF\"\n", data.dat_file_name_parameters, loop_num - plot_width, loop_num);

  	// プロット：探索欲求
  	// fprintf(gid, "set title \"inner states for EXPLORE\"\n");
  	fprintf(gid, "plot \"%s\" using 1:20 every ::%d::%d with lines title \"Explore\" lw 5 lc \"#80FF00\"\n", data.dat_file_name_parameters, loop_num - plot_width, loop_num);

  	// 出力
  	fprintf(gid, "unset multiplot\n");  // 並べて表示ここまで
  	fflush(gid);
  }
}



// グラフ表示：gnuplot
void DebugClass::plotBehabior()
{
	// 仮処置
	int loop_num = *this->loop_num;

  // if(1&&loop_num % 3 == 0)  // 100msごとに描画するとチラついてしまう
  {
  	// 準備
  	int plot_width = 1000, plot_margin = 0;
  	// fprintf(gnuplot_b, "set multiplot layout 1,2 rowsfirst downwards\n");  // 並べて表示ここから
  	// fprintf(gnuplot_b, "set title font \"Helvetica, 13\"\n");  // タイトル
  	// fprintf(gnuplot_b, "set key left top\n");  // 凡例の位置
		fprintf(gnuplot_b, "set nokey\n");  // 凡例なし
  	fprintf(gnuplot_b, "set yrange [0:%d]\n", 5);  // y軸の範囲
  	fprintf(gnuplot_b, "set xrange [%d:%d]\n", loop_num + plot_margin - plot_width, plot_margin + loop_num);  // x軸の範囲

		// char behavior_name[7][32] = {"GO TO SITTING OWNER", "PLAY USING BALL", "EXPLORE", "MISSING", "GO TO DOOR", "GO TO OWNER", "EXPLORE STRANGER"};
		char behavior_name[6][32] = {"GO TO SITTING OWNER", "EXPLORE", "MISSING", "GO TO DOOR", "GO TO OWNER", "EXPLORE STRANGER"};

		char command[256];
		sprintf(command, "set ytics (");
		for(int i=0; i<6; i++) sprintf(command, "%s\"%s\" %d, ", command, behavior_name[i], i);
		sprintf(command, "%s)\n", command);

		fprintf(gnuplot_b, command);

  	fprintf(gnuplot_b, "plot \"%s\" using 1:21 every ::%d::%d with points pt 5 ps 1 lc \"#FF0080\" title \"Behavior\"\n", data.dat_file_name_parameters, loop_num - plot_width, loop_num);

  	// 出力
  	// fprintf(gnuplot_b, "unset multiplot\n");  // 並べて表示ここまで
  	fflush(gnuplot_b);
  }
}


// グラフ表示：gnuplot
void DebugClass::saveDat_plot3d()
{
	// 仮処置
	int loop_num = *this->loop_num;

	// ファイルへの出力
	std::ofstream log_file_3d(data.dat_file_name_3d, std::ios::trunc);
	// データセット
	char send_message_3d[1024] = "";
	// ヘッダ
	sprintf(send_message_3d, "#X\tY\tMiss\tAnxiety\tExploreH\tExploreP\tSum\tStress");
	log_file_3d << send_message_3d << std::endl;
	// 書き込み
	for(int i=0; i<gridnum1; i++)
	{
		for(int j=0; j<gridnum2; j++)
		{
			sprintf(send_message_3d, "%3d\t%3d\t%10.5lf\t%10.5lf\t%10.5lf\t%10.5lf\t%10.5lf\t%10.5lf",
			 		i, gridnum2-j,
					data.dMiss_score[i][j],
					data.dAnxiety_score[i][j],
					data.dExploreHuman_score[i][j],
					data.dExplorePlace_score[i][j],
					data.dSum_score[i][j],
					data.stress_parm_grid[i][j] );
			log_file_3d << send_message_3d << std::endl;
		}
		log_file_3d << std::endl;
	}
	// ログファイルを閉じる
	log_file_3d.close();
}

void DebugClass::plot3d()
{
	// プロット
	fprintf(gnuplot_3d, "set ticslevel 0\n");
	fprintf(gnuplot_3d, "set palette rgbformula 22,13,-31\n");

	if(*plot_type == 0)
	{
		fprintf(gnuplot_3d, "set title \"Miss + Anxiety + Explore Human + Explore Place\"\n");
	}
	if(*plot_type == 1)
	{
		fprintf(gnuplot_3d, "set title \"Miss + Anxiety + Explore Human + Explore Place + Move Stress\"\n");
	}

	fprintf(gnuplot_3d, "splot \"%s\" using 1:2:%d with pm3d\n", data.dat_file_name_3d, *plot_type + 7);
	fflush(gnuplot_3d);
}

void DebugClass::plot3dmap()
{
	fprintf(gnuplot_3d_1, "set ticslevel 0\n");
	fprintf(gnuplot_3d_1, "set palette rgbformula 22,13,-31\n");
	fprintf(gnuplot_3d_1, "set pm3d map\n");
	fprintf(gnuplot_3d_1, "set title \"Miss\"\n");
	fprintf(gnuplot_3d_1, "splot \"%s\" using 1:2:3\n", data.dat_file_name_3d);
	fflush(gnuplot_3d_1);
  //
	fprintf(gnuplot_3d_2, "set ticslevel 0\n");
	fprintf(gnuplot_3d_2, "set palette rgbformula 22,13,-31\n");
	fprintf(gnuplot_3d_2, "set pm3d map\n");
	fprintf(gnuplot_3d_2, "set title \"Anxiety\"\n");
	fprintf(gnuplot_3d_2, "splot \"%s\" using 1:2:4\n", data.dat_file_name_3d);
	fflush(gnuplot_3d_2);

	fprintf(gnuplot_3d_3, "set ticslevel 0\n");
	fprintf(gnuplot_3d_3, "set palette rgbformula 22,13,-31\n");
	fprintf(gnuplot_3d_3, "set pm3d map\n");
	fprintf(gnuplot_3d_3, "set title \"Explore Human\"\n");
	fprintf(gnuplot_3d_3, "splot \"%s\" using 1:2:5\n", data.dat_file_name_3d);
	fflush(gnuplot_3d_3);

	fprintf(gnuplot_3d_4, "set ticslevel 0\n");
	fprintf(gnuplot_3d_4, "set palette rgbformula 22,13,-31\n");
	fprintf(gnuplot_3d_4, "set pm3d map\n");
	fprintf(gnuplot_3d_4, "set title \"Explore Place\"\n");
	fprintf(gnuplot_3d_4, "splot \"%s\" using 1:2:6\n", data.dat_file_name_3d);
	fflush(gnuplot_3d_4);
}


void DebugClass::plotLoop()
{
	while(thread_continue_flag)
	{
		// パラメータ変化グラフ
		plotParameters();
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		saveDat_plot3d();
		if(*plot_type == 2)
		{
			// 愛着行動のストレス増減マップ（平面四つ）
			plot3dmap();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		else
		{
			// 愛着行動のストレス増減マップ（立体一つ）
			plot3d();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));

			// ふるまい履歴
			plotBehabior();
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
}

void DebugClass::ThreadBegin(int *loop_num, int *plot_type)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(100));

  prepare_plotParameters();
  prepare_plot3d();
	prepare_plot3dmap();

	prepare_plotBehavior();

	thread_1 = new std::thread(&DebugClass::plotLoop, this);
	this->loop_num = loop_num;
	this->plot_type = plot_type;
}

void DebugClass::ThreadFinish()
{
	if(thread_1==nullptr)
	{
		PRINT("the thread instance had not activated");
	}
	else
	{
		thread_continue_flag = false;
		thread_1->join();
		thread_1=nullptr;
		PRINT("the thread instance has successfully removed");
	}
}


DebugClass::DebugClass()
{
  prepare_printDebug();
  prepare_logSave();
}


DebugClass::~DebugClass()
{
	// スレッド終了
	ThreadFinish();

	// gnuplot
	pclose(gid);  // gnuplot終了（この前にexitを読んでも意味ない模様）

	// gnuplot3d
	pclose(gnuplot_3d);  // gnuplot終了（この前にexitを読んでも意味ない模様）

	pclose(gnuplot_3d_1);  // gnuplot終了
	pclose(gnuplot_3d_2);  // gnuplot終了
	pclose(gnuplot_3d_3);  // gnuplot終了
	pclose(gnuplot_3d_4);  // gnuplot終了
}
