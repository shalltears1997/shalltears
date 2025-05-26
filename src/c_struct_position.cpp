#pragma once

#include <iostream>
#include <cmath>

#include "define.hpp"
#include "c_struct_position.hpp"


Vec InitVec()
{
	Vec initialize;
	initialize.vx = 0.0;
	initialize.vy = 0.0;
	return initialize;
}

Position InitPose()//構造体Positionの初期化関数
{
	Position initialize;
	initialize.posex = 0.0;
	initialize.posey = 0.0;
	initialize.posez = 1000.0;
	initialize.deg = 0.0;
	return initialize;
}

//------------------------------------------------------------------------

//calcuration
double calDistance(double dx , double dy)
{
	return sqrt(pow(dx, 2) + pow(dy, 2));
}
double calDistance(Position target, double pos_x,double pos_y)
{
	return sqrt(pow(target.posex-pos_x, 2) + pow(target.posey-pos_y, 2));
}
double calDistance(Position target1 , Position target2 )
{
	return sqrt(pow(target1.posex-target2.posex, 2) + pow(target1.posey-target2.posey, 2));
}
double calDegree(Position viewpoint , Position target)
{
	double delta_x = target.posex - viewpoint.posex;
	double delta_y = target.posey - viewpoint.posey;
	return atan2(delta_y , delta_x)*(180.0/PI);
}
double calDegree(Position viewpoint ,double target_x ,double target_y)
{
	double delta_x = target_x - viewpoint.posex;
	double delta_y = target_y - viewpoint.posey;
	return atan2(delta_y , delta_x)*(180.0/PI);
}
double calExp(double distance)
{
	return  pow(2.71828 , distance);
}
double calX(double distance)
{
	return  1 / distance;
}
Position addPose(Position object1 , Position object2)
{
	Position answer = InitPose();
	answer.posex = object1.posex + object2.posex;
	answer.posey = object1.posey + object2.posey;
	answer.posez = object1.posez + object2.posez;

	return answer;
}

Position subtructPose(Position object1 , Position object2)
{
	Position answer = InitPose();
	answer.posex = object1.posex - object2.posex;
	answer.posey = object1.posey - object2.posey;
	answer.posez = object1.posez - object2.posez;

	return answer;
}

Position subtructPose(double object1_x , double object1_y , Position object2)
{
	Position answer = InitPose();
	answer.posex = object1_x - object2.posex;
	answer.posey = object1_y - object2.posey;

	return answer;
}

Position subtructPose2D(Position object1 , double object2_x ,double object2_y)
{
	Position answer = InitPose();
	answer.posex = object2_x - object1.posex;
	answer.posey = object2_y - object1.posey;

	return answer;
}

Position multiplyPose(double factor_val ,  Position object)
{
	object.posex *= factor_val;
	object.posey *= factor_val;
	object.posez *= factor_val;

	return object;
}

Position dividePose(double factor_val , Position object)
{
	object.posex /= factor_val;
	object.posey /= factor_val;
	object.posez /= factor_val;

	return object;
}

Position unitVector2D(Position target , Position viewpoint)
{
	Position answer = InitPose();
	double distance = calDistance(target , viewpoint);
	answer = dividePose(distance , subtructPose(target , viewpoint));

	return answer;
}
Position unitVector2D(double targetX , double targetY , Position viewpoint)
{
	Position answer = InitPose();
	double distance = calDistance(viewpoint , targetX , targetY);
	answer = dividePose(distance , subtructPose2D(viewpoint , targetX , targetY) );

	return answer;
}
double calVecVal(Vec vec)
{
	return sqrt(vec.vx*vec.vx + vec.vy * vec.vy);
}

Vec calVec(Position target , Position viewpoint)
{
	Vec answer = InitVec();
	answer.vx = target.posex - viewpoint.posex;
	answer.vy = target.posey - viewpoint.posey;
	return answer;
}

Vec devideVec(double factor_val , Vec vec)
{
	vec.vx /= factor_val;
	vec.vy /= factor_val;
	return vec;
}

Vec unitVector(Position target , Position viewpoint)
{
	Vec answer = calVec(target , viewpoint);
	answer = devideVec(calVecVal(answer), answer);
	return answer;
}
double inProduct(Vec vec1, Vec vec2)
{
	return vec1.vx*vec2.vx + vec1.vy*vec2.vy;
}

double calTheta(Vec vec1 , Vec vec2) //degree;
{
	double cosine;
	double answer;
	cosine = inProduct(vec1 , vec2)/(calVecVal(vec1) * (calVecVal(vec2)));
	answer = acos(cosine)*180.0/PI;
	return answer;
}
double calTheta(Vec vec)
{
	return atan2(vec.vy , vec.vx)*180.0/PI;
}


double orthoDeg(double delta_y , double delta_x)
{
	double radian = atan(delta_y/delta_x);
	if(delta_x < 0.0){
		return (radian - PI/2.0)*(180.0/PI);
	}
	else{
		return (radian + PI/2.0)*(180.0/PI);
	}
}

double pid_posi(double now_posi, double target_posi, double dt)//soh追加
{
	double P, I, D;
	static double integral = 0.0;
	static double diff[2] = {0.0};
	double Kp = 0.7;
	double Ki = 0.6;
	double Kd = 0.01;
	double pid_val = 0.0;
	static bool first = true;
	if(first){
		first = false;
		return 0.0;
	}
	else{
		diff[0] = diff[1];
		diff[1] = target_posi - now_posi;
		integral += (diff[1] + diff[0]) / 2.0 * dt;


		if(integral >= 5.0){
			integral = 5.0;
		}
		else if(integral <= -5.0){
			integral = -5.0;
		}
		//P
		P = Kp * diff[1];
		//I
		I = Ki * integral;
		//D
		D = Kd * (diff[1] - diff[0]) / dt;

		pid_val = P+I+D;
	}
	return pid_val;
}
