#pragma once

//構造体：Position
typedef struct
{
	double posex;
	double posey;
	double posez;
	double deg;
}Position;

typedef struct
{
	double vx;
	double vy;
}Vec;

Vec InitVec();
Position InitPose();	//構造体Positionの初期化関数

//------------------------------------------------------------------------

//calcuration
double calDistance(double dx , double dy);
double calDistance(Position target, double pos_x, double pos_y);
double calDistance(Position target1, Position target2);
double calDegree(Position viewpoint , Position target);
double calDegree(Position viewpoint ,double target_x ,double target_y);
double calExp(double distance);
double calX(double distance);
Position addPose(Position object1 , Position object2);
Position subtructPose(Position object1 , Position object2);
Position subtructPose(double object1_x , double object1_y , Position object2);
Position subtructPose2D(Position object1 , double object2_x ,double object2_y);
Position multiplyPose(double factor_val ,  Position object);
Position dividePose(double factor_val , Position object);
Position unitVector2D(Position target , Position viewpoint);
Position unitVector2D(double targetX , double targetY , Position viewpoint);
double calVecVal(Vec vec);
Vec calVec(Position target , Position viewpoint);
Vec devideVec(double factor_val , Vec vec);
Vec unitVector(Position target , Position viewpoint);
double inProduct(Vec vec1, Vec vec2);
double calTheta(Vec vec1 , Vec vec2);	//degree
double calTheta(Vec vec);
double orthoDeg(double delta_y , double delta_x);
double pid_posi(double now_posi, double target_posi, double dt);	//soh追加
Position adjust_point(Position place);
