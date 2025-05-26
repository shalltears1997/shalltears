#pragma once

#include <vector>
#include <chrono>

#include "define.hpp"
#include "c_struct_position.hpp"
#include "tk_data_struct.hpp"

#include "l_tk_pote.hpp"


class ParameterClass
{
public:

  std::vector<double> attachment;
	std::vector<double> miss;
	std::vector<double> anxiety;
};



class DataClass
{
public:
  DataClass()
  {
    std::vector<DATA_human> tmp(human_num);
    human = tmp;
    human[0].posi = {1500, 1500};
    human[1].posi = {4850, 4000};
    // human[2].posi = {4850, 3500};
    human[2].posi = {5000, 5000};
    human[0].height = 1500;
    human[1].height = 1500;
    human[2].height = 1500;

    std::vector<double> tmp1(human_num, INFINITY);
    human_non_exist_duration = tmp1;

    robot.posi = {-3000, 0};

		std::vector<std::vector<double>> tmp2(gridnum1, std::vector<double>(gridnum2, 0));
		stress_pote = tmp2;

    std::vector<std::vector<double>> tmp3(gridnum1/10, std::vector<double>(gridnum2/10, 0));
    unknown_area = tmp3;

    std::vector<std::vector<double>> tmp4(gridnum1, std::vector<double>(gridnum2, 0));
    std::vector<std::vector<float>> tmp6(gridnum1, std::vector<float>(gridnum2, 0));

    dMiss_score = tmp4;
    dAnxiety_score = tmp4;
    dExploreHuman_score = tmp4;
    dExplorePlace_score = tmp4;

    dSum_score = tmp4;
    stress_parm_grid = tmp4;

    std::vector<std::vector<double>> tmp5(gridnum1, std::vector<double>(gridnum2, 0));
    unknown_area_finer = tmp5;
  }
  // Position po_owner(){return po_human[0];};
  // Position po_semi_owner(){return po_human[0];};
  // Position po_non_owner(int n){return po_human[2];};

  // ランダムなオーナー/ストレンジャー選択
  int OSinverse = 0;
  bool home = 0;

  //////////////////////////////////////////////////////////////////////////////
  // システム
  //////////////////////////////////////////////////////////////////////////////
  char start_time[16] = "";
  std::vector<DATA_grid_part> low_grids;

  // デバッグ出力のため
  char dat_file_name_parameters[128] = "";
  char dat_file_name_3d[128] = "";
  char dat_file_name_gnuplot[128] = "";

  int loop_num = 0;
  int plot_type = 0;



  // Macメインディスプレイ（ターミナルサイズは 95x18）
  int display_w = 1440, display_h = 900;
  int display_x = 0, display_y = 0;
  int display_x_from_main = 0, display_y_from_main = 0;

  // 大学自席の大きいディスプレイ（ターミナルサイズは 127x22）
  // int display_w = 1920, display_h = 1080;
  // int display_x = 0, display_y = 0;
  // int display_x_from_main = 1440 - 1920, display_y_from_main = -display_h;

  // 大学の中くらいのディスプレイ（ターミナルサイズは 111x22）
  // int display_w = 1680, display_h = 1050;
  // int display_x = 1920, display_y = 0;
  // int display_x_from_main = 1440, display_y_from_main = -1080;

  // 大学のCog班のところの一番右のディスプレイ、新しい4Kディスプレイ（ターミナルサイズは 127x22）
  // int display_w = 1920, display_h = 1080;
  // int display_x = 0, display_y = 0;
  // int display_x_from_main = 0, display_y_from_main = -display_h;



#if defined(_WIN32) || defined(_WIN64)
  // Windows

  // 下部バーの高さ
  double display_margin_top = 0;
  // 左部バーの幅
  double display_margin_left = 0;
  // ウィンドウの上部バーの高さ
  double window_margin_top = 0;

#elif defined(__APPLE__)
  // Mac

  // 上部バーの高さ
  double display_margin_top = 22;
  // 左部バーの幅
  double display_margin_left = 0;
  // ウィンドウの上部バーの高さ
  double window_margin_top = 23;

#else
  // Ubuntu

  // 上部バーの高さ
  double display_margin_top = 24;
  // 左部バーの幅
  double display_margin_left = 65;
  // ウィンドウの上部バーの高さ
  double window_margin_top = 28;

#endif

  // 描画領域サイズ
  double field_w = display_w - display_margin_left;
  double field_h = display_h - display_margin_top;


  // glの切り取りサイズ：glOrtho
	// int gl_x0 = -3300, gl_x1 = 5000;
	// int gl_y0 = -1500, gl_y1 = 4000;
	int gl_x0 = -3300, gl_x1 = 5250;
	int gl_y0 = -1100, gl_y1 = 4800;


  // シミュレータ描画割合
  double sim_ratio_w = 0.6;
  // double sim_ratio_h = 0.6;
  double sim_ratio_h = (sim_ratio_w * (double)display_w + window_margin_top) * ((double)(gl_y1 - gl_y0) / (double)(gl_x1 - gl_x0)) / (double)field_h;

  // glのウィンドウサイズ
	double gl_w = field_w * sim_ratio_w;
	double gl_h = field_h * sim_ratio_h - window_margin_top;


  // plot描画割合
  double plot_ratio_h = 1.0/3.0;
  double h_score = 4.5/10.0;
  double h_behavior = 2.0/10.0;
  double h_parameters = 3.5/10.0;
  double h_map4x = 6.5/10.0;


  // GLキャプチャ保存
  double time_duration;
  int fig_loop = 0;


  //////////////////////////////////////////////////////////////////////////////
  // 入力
  //////////////////////////////////////////////////////////////////////////////
  // ロボット
  // Position robot;
  DATA_robot robot;
  // 人
  int human_num = 2;
  std::vector<DATA_human> human = { {{0,0},0,0,0}, {{0,0},0,0,0}, {{0,0},0,0,0} };
  std::vector<DATA_xy> human_vec = { {0, 0}, {0, 0}, {0, 0} };
  // std::vector<bool> human_move = {false, false, false};

  DATA_object object[100]={};

  DATA_object potedama = {{-1500, 2800},0,0,0};

  //一番監視が必要な動物体のIDを保持
  int object_maxID = 0;
  double max_necessity_obj = 0.0;

  DATA_xy wall_edge = {-2000,2000};

  std::vector<double> human_non_exist_duration;

  double shagamu_th = 550;

  // 人が領域から出た場所
  DATA_xy *human_leave_location = nullptr;

  // 環境地図
  // 障害物
  DATA_obstacle obstacle[10] = {{-2500,3000,-2000,1500}, {-1500,1500,-1000,0}};
  // ボールの見える場所など
  BALL ball_st;
  double ball_last_time = -INFINITY;
  // 音方向
  bool sound_move = false;
  // コマンド
  bool GoToHomeFlag = false;

  double pan, tilt;


  DATA_xy tmp_goal;

  bool robot_move_or_halt = false;

  //////////////////////////////////////////////////////////////////////////////
  // 特殊１：入力でもあり、出力でもある。面倒なので、GLからもdataを直接覗く＝GLDataで扱わない
  //////////////////////////////////////////////////////////////////////////////
  // 行動特性パラメータ
  double sensitivity_of_anxiety = 50;
  // システムパラメータ
  bool zps_robot_new = false;
  NAZO nazo;

  //////////////////////////////////////////////////////////////////////////////
  // 特殊２：固定値
  //////////////////////////////////////////////////////////////////////////////
  Position Door_pose = {2900, 2900, 1000, 0};


  //////////////////////////////////////////////////////////////////////////////
  // 出力（動作）
  //////////////////////////////////////////////////////////////////////////////
  bool is_new = false;

  // ストレス(先頭値が信頼度)
  PARAMETER pa_human[3] = { {100, 0,0,0,0,0,0, 0,0,0, 0,100,0, 0,100,0},
                            {0, 0,0,0,0,0,0, 0,0,0, 0,0,0, 0,0,0} };
  double explore = 0;

  int last_MorA;

  // 忘却率の時定数
  double TimeConstant1 = 30;
  double TimeConstant2 = 20;

  // 感受性
  double K_anx = 1.0;
  double K_exp = 1.0;
  double K_mis = 1.0;

  // ふるまい変更可否レベル
  int ball_play_required_level = 0;

  // 未知領域
  std::vector<std::vector<double>> unknown_area;
  std::vector<std::vector<double>> unknown_area_finer;

  std::vector<std::vector<double>> dMiss_score;
  std::vector<std::vector<double>> dAnxiety_score;
  //std::vector<std::vector<double>> dAnxiety_move;
  std::vector<std::vector<double>> dExploreHuman_score;
  std::vector<std::vector<double>> dExplorePlace_score;

  std::vector<std::vector<double>> dSum_score;
  std::vector<std::vector<double>> stress_parm_grid;

  // std::vector<DATA_grid_part> stress_min;
  // ストレスポテンシャル
  std::vector<std::vector<double>> stress_pote;
  // 経路生成ポテンシャル
  PotentialFunction pf_goal;

  //監視必要度 monitoring_necessity
 double monitoring_necessity[6][3] = {};
 float weigth_area[6][3] = {};
 double max_necessity_area = 0.0;
 DATA_grid max_area_grid = {};

 bool flag_attention=false;//attentiongetting;
bool flag_direction=false;
int count =0;
  //活動度合い
 float weight[60][30] = {};


  // 探索履歴
  // ボール
  BALL_yoheisan ball_yo;
  DATA_xy ball_tk = {4850, 3000};
  bool ball_is_Known_tk = false;

  // モード選択
  int mode = 0;
  // 行動選択
  int motion_id;
  // 動作
  COMMAND_set command;
  DATA_xy sub_goal;
  bool pioneer_activate = false;
};


class DebugDataClass
{
public:
  std::vector<DATA_grid_part> low_grids;

};


class ConfigDataClass
{
public:
  float shagamu = 600;//700;
  char cwd[255];

 //  90 地面
 // 400 しゃがんで前かがみ
 // 940 しゃがむ
 // 1200 椅子に座る
 // 1500 立つ（阿部くん）
 //
 // 1100を閾値にしよう！

};
