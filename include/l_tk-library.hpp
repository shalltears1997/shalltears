#pragma once
#include "class.hpp"

// デバッグ用
#ifndef PRINT
#define PRINT(...) Print(__func__, __FILE__, __LINE__, __VA_ARGS__)
#endif
// #define dprintf(...) printf(__VA_ARGS__)


// 最大8192バイトの文字を表示する関数
void Print(const char func[], char file[], int line, const char *format, ...);

double calDistance(DATA_xy target1, DATA_xy target2);
double calDistance(Position target1, DATA_xy target2);
double calDistance(DATA_xy target1, Position target2);
double calDegree(DATA_xy viewpoint, DATA_xy target);


Human* human2owner(int owner_flag, Human *human_0, Human *human_1, Human *human_2);
Human* human2stranger(int owner_flag, Human *human_0, Human *human_1, Human *human_2);
Human* human2stranger2(int owner_flag, Human *human_0, Human *human_1, Human *human_2);

int ownerVerifier(int owner_flag);
int strangerVerifier(int owner_flag);
int stranger2Verifier(int owner_flag);

double grid2double_x(int i);
double grid2double_y(int j);
double grid2double(int i);
DATA_xy grid2double(DATA_grid point);

int double2grid(double x);
int double2grid_x(double x);
int double2grid_y(double y);

bool grid_in_judge(int i,int j, DATA_xy robot);

DATA_xy grid_to_coordinate(DATA_grid areapoint);
double grid_to_coordinate_y(int j);
double grid_to_coordinate_x(int i);
DATA_grid grid(DATA_xy position);

DATA_xy adjust_point(DATA_xy place);
Position adjust_point(Position place);
Position adjust_point_new(DATA_grid place);
bool exist_judge_human(DATA_xy place);
bool exist_judge_robot(DATA_xy place);

double attachment2distance_tmp(int attachment);

bool judge_in_ellipse(double x,double y);
bool visible_area(DATA_xy place);
DATA_grid owner_grid(DATA_xy posi);
