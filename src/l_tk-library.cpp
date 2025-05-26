#pragma once

#include <cstdlib>
#include <cmath>
#include <cstring>
#include <iostream>
#include "opengl.hpp"

#include "define.hpp"
#include "gv_extern_id.hpp"

#include "c_struct_position.hpp"
#include "l_tk_pote.hpp"
#include "class.hpp"


#ifndef gridnum1
#define gridnum1 60
#endif

#ifndef gridnum2
#define gridnum2 30
#endif


// ターミナルのサイズ取得
#include <sys/ioctl.h>
#include <unistd.h>
#include <cmath>

// printfのラッパ
#include <stdarg.h>

void print_one_line(char buff[])
{
	// 日本語一文字のバイト数
	// 一般的な「ひらがな」「漢字」のバイト数は、文字コードによって決まる
	// （一部の「ひらがな」「漢字」はそれと異なることがあるが、無視する）
	int jp_byte = strlen("あ");
	// printf("jp_byte: %d\n", jp_byte);

	int byte_all = strlen(buff) - 1;  // 終端記号分を差し引いて文字数とする

	// ASCII以外の文字数
	int n_ascii = 0;
	for(int i=0; i<byte_all; i++) if(buff[i]>>7==0) n_ascii++;
	// printf("n_ascii: %d\n", n_ascii);

	// 文字列に含まれる日本語の文字数
	int n_jp = (byte_all - n_ascii) / jp_byte;
	// printf("n_jp: %d\n", n_jp);

	// 文字列に含まれるすべての文字数（一応だしてみた）
	int n_all = n_ascii + n_jp;
	// printf("n_all: %d\n", n_all);

	// 文字列が占める幅（ASCII=1、日本語=2として算出）
	int width_all = n_ascii + n_jp * 2;
	// printf("width: %d\n", width_all);

	// ターミナルのサイズ取得
	struct winsize ws;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
	// printf("terminal width  %d\n", ws.ws_col);
	// printf("terminal height %d\n", ws.ws_row);

	// 文字列が必要とするターミナルの行数
	int n_line = (width_all - 1) / ws.ws_col + 1;
	// printf("n_line: %d\n", n_line);

	// ログ表示分、領域を確保（下にスクロールする）
	// printf("\033[%dS", n_line);
	for (int i=0; i<n_line; i++) printf("\n");
	// スクロール分 + 必要な行数分、カーソルを上に移動
	printf("\033[%dF", ESCAPE_OVER_NUM + n_line);
	// 必要な行数分、行を挿入
	printf("\033[%dL", n_line);
	// 内容表示
	printf("%s", buff);
	// ログ表示分下へカーソル移動（一番下に戻す）
	printf("\033[%dE", ESCAPE_OVER_NUM + 1);
	fflush(stdout);
}

// 最大8192バイトの文字を表示する関数
void Print(const char func[], char file[], int line, const char *format, ...)
{
	// 単純に表示するだけならこう
	// va_list va;
	// va_start(va, format);
	// // int vprintf(const char *format, va_list ap);
	// vprintf(format, va);
	// va_end(va);

	char str_in[8192];
	char *str_split;
	char str_debug[8192];
	int i = 0;


	// 文字列の全バイト数
	va_list va;
	va_start(va, format);
	vsprintf(str_in, format, va);
	va_end(va);

	// 分割（最初）
	str_split = strtok(str_in, "\n");

	// 表示と、次のための分割（2回目以降）
	do
	{
		// 呼び出し場所の追加（デバッグ用）
		sprintf(str_debug, "( %s() at %s: %d ) %s", func, file, line, str_split);

		// 出力
		print_one_line(str_debug);
		// printf("%s\n", split);

		// 分割（2回目以降）
		i++;
		str_split = strtok(NULL, "\n");
	}
	while(str_split!=NULL && i+1<128-1);

	// '\n'の数
	// printf("split: %d\n", i);  // i-1+1が改行の数（-1はNULLの分、+1は0はじまりの分）
}


// DATA_xy
double calDistance(DATA_xy target1, DATA_xy target2)
{
	return sqrt( pow((target1.x-target2.x),2) + pow((target1.y-target2.y),2) );
}
// Position と DATA_xy の混合
double calDistance(Position target1, DATA_xy target2)
{
	return sqrt( pow((target1.posex-target2.x),2) + pow((target1.posey-target2.y),2) );
}
double calDistance(DATA_xy target1, Position target2)
{
	return sqrt( pow((target1.x-target2.posex),2) + pow((target1.y-target2.posey),2) );
}

double calDegree(DATA_xy viewpoint, DATA_xy target)
{
	return atan2(target.y-viewpoint.y, target.x-viewpoint.x)*(180.0/PI);
}

Human* human2owner(int owner_flag, Human* human_0, Human *human_1, Human *human_2)
{
	switch(owner_flag)
	{
		case 0:
			return human_0;
			break;
		case 1:
			return human_1;
			break;
		case 2:
			return human_2;
			break;
	}
}

Human* human2stranger(int owner_flag, Human* human_0, Human *human_1, Human *human_2)
{
	switch(owner_flag)
	{
		case 0:
			return human_1;
			break;
		case 1:
			return human_0;
			break;
		case 2:
			return human_0;
			break;
	}
}


Human* human2stranger2(int owner_flag, Human* human_0, Human *human_1, Human *human_2)
{
	switch(owner_flag)
	{
		case 0:
			return human_2;
			break;
		case 1:
			return human_2;
			break;
		case 2:
			return human_1;
			break;
	}
}

int ownerVerifier(int owner_flag)
{
	switch(owner_flag)
	{
		case 0:
			return 0;
			break;
		case 1:
			return 1;
			break;
		case 2:
			return 2;
			break;
	}
}

int strangerVerifier(int owner_flag)
{
	switch(owner_flag)
	{
		case 0:
			return 1;
			break;
		case 1:
			return 0;
			break;
		case 2:
			return 0;
			break;
	}
}

int stranger2Verifier(int owner_flag)
{
	switch(owner_flag)
	{
		case 0:
			return 2;
			break;
		case 1:
			return 2;
			break;
		case 2:
			return 1;
			break;
	}
}





// 本当はこれは別の場所で定義したほうがいいはず
double grid_length = (double)(WIDTH_ETHO/gridnum1);


// そのグリッドの中央の座標を返す
double grid2double_x(int i)
{
  return (i-gridnum1/2)*100 + grid_length/2;
}
double grid2double_y(int j)
{
  return (gridnum2-1-j)*100 + grid_length/2;
}
double grid2double(int i)
{
  return i*100 + grid_length/2;
}

double grid_to_coordinate_x(int i)
{
	return (i-6/2)*1000 + 1000/2;
}

double grid_to_coordinate_y(int j)
{
	return (3-1-j)*1000 + 1000/2 - 500;
}

bool grid_in_judge(int i,int j,DATA_xy robot)
{
	if(robot.x < grid_to_coordinate_x(i)-500) return false;
	if(robot.x > grid_to_coordinate_x(i)+500) return false;
	if(robot.y < grid_to_coordinate_y(j)-500) return false;
	if(robot.y > grid_to_coordinate_y(j)+500) return false;

	return true;
}

DATA_xy grid_to_coordinate(DATA_grid areapoint)
{
	return {grid_to_coordinate_x(areapoint.i),grid_to_coordinate_y(areapoint.j)};
}

DATA_xy grid2double(DATA_grid point)
{
	return {grid2double_x(point.i), grid2double_y(point.j)};
}

// 計算後に四捨五入して近傍のグリッド番号を返す
int double2grid(double x)
{
  //return round(x/grid_length);
  return (int)(x/grid_length);
}
int double2grid_x(double x)
{
  //int i = round((x+RoomPoint[1])/grid_length);
  int i = (int)((x+RoomPoint[1])/grid_length);
//	if(i == gridnum1) i--;

	// if(i > gridnum1-2) i = gridnum1-2;
	// if(i < 1) i = 1;
  return i;
}
int double2grid_y(double y)
{
  //int j = round((RoomPoint[2]-y)/grid_length);
  int j = (int)((RoomPoint[2]-y)/grid_length);
//  if(j == gridnum2) j--;

  // if(j > gridnum2-2) j = gridnum2-2;
  // if(j < 1) j = 1;
  return j;
}


Position adjust_point(Position place)
{
//	int region_xXyY[4] = {-3000, 3000, 0, 3000}; //本来
//	int region_xXyY[4] = {-2500, 2500, 500, 2500}; //現実的（実験）
//	int region_xXyY[4] = {-3000, 0, 600, 2000};

	int region_xXyY[4] = {-2999, 2999, 1, 2999};

	if(place.posex < region_xXyY[0]) place.posex = region_xXyY[0];
	if(place.posex > region_xXyY[1]) place.posex = region_xXyY[1];
	if(place.posey < region_xXyY[2]) place.posey = region_xXyY[2];
	if(place.posey > region_xXyY[3]) place.posey = region_xXyY[3];

	return place;
}



DATA_xy adjust_point(DATA_xy place)
{
	int region_xXyY[4] = {-3000, 3000, 0, 3000};
	if(place.x < region_xXyY[0]) place.x = region_xXyY[0];
	if(place.x > region_xXyY[1]) place.x = region_xXyY[1];
	if(place.y < region_xXyY[2]) place.y = region_xXyY[2];
	if(place.y > region_xXyY[3]) place.y = region_xXyY[3];
	return place;
}


Position adjust_point_new(DATA_grid place)
{
//	int region_xXyY[4] = {-3000, 3000, 0, 3000}; //本来
//	int region_xXyY[4] = {-2500, 2500, 500, 2500}; //現実的（実験）
//	int region_xXyY[4] = {-3000, 0, 600, 2000};

	int region_xXyY[4] = {-2999, 2999, 1, 2999};

	double x = grid2double_x(place.i);
	double y = grid2double_y(place.j);

	if(x < region_xXyY[0]) x = region_xXyY[0];
	if(x > region_xXyY[1]) x = region_xXyY[1];
	if(y < region_xXyY[2]) y = region_xXyY[2];
	if(y > region_xXyY[3]) y = region_xXyY[3];

	return {x, y, 0, 0};
}


bool exist_judge_human(DATA_xy place)
{
	int region_xXyY[4] = {-3000, 4500, 0, 4500}; //本来
	// int region_xXyY[4] = {-3000, 3000, 0, 3000}; //本来

	if(place.x < region_xXyY[0]) return false;
	if(place.x > region_xXyY[1]) return false;
	if(place.y < region_xXyY[2]) return false;
	if(place.y > region_xXyY[3]) return false;

	return true;
}

bool exist_judge_robot(DATA_xy place)
{
	int region_xXyY[4] = {-3000, 3000, 0, 3000}; //本来

	if(place.x < region_xXyY[0]) return false;
	if(place.x > region_xXyY[1]) return false;
	if(place.y < region_xXyY[2]) return false;
	if(place.y > region_xXyY[3]) return false;

	return true;
}

double attachment2distance_tmp(int attachment)
{
	double d_max = 1250, d_min = 750;
	int a_max = 100;
	double distance = (double)(a_max - attachment)/a_max * (d_max - d_min) + d_min;
	return distance;
}

bool visible_area(DATA_xy place)
{
	int region_xXyY[4] = {-3000, -2000, 0, 3000}; //本来

	if(place.x < region_xXyY[0]) return false;
	if(place.x > region_xXyY[1]) return false;
	if(place.y < region_xXyY[2]) return false;
	if(place.y > region_xXyY[3]) return false;

	return true;
}

bool judge_in_ellipse(double x,double y)
{
	double elli =0.0;

	elli = ((x + 2000)*(x + 2000) / (500*500)) + ((y - 2500)*(y - 2500) / (800*800));

	if(elli < 1.0)
	{
		return true;
	}

	return false;
}

DATA_grid owner_grid(DATA_xy posi)
{
	int i,j;
	if(posi.x>=-3000 && posi.x<= 3000 && posi.y>= 0 && posi.y <=3000)
	{
		i = (int)((posi.x + 3000)/1000);
		j = (int)((3000 - posi.y)/1000);
		return {i,j};
	}
	else return {-1,-1};

}

