#include <cstring>
#include <cmath>
#include <iostream>
#include <chrono>
#include <thread>
#include "opengl.hpp"

#include "gv_extern.hpp"

#include "c_struct_position.hpp"
#include "l_tk_pote.hpp"
#include "c_human.hpp"

Human::Human(double value ,char type[])
{
	current = InitPose();
	previous = InitPose();
	attachment = value;
	strcpy(name , type);
	data[0] = 0.0;
	data[1] = 0.0;
	data[2] = 0.0;
	data[3] = 0.0;
	will = 0;
	first_enter = true;
}


bool Human::exist(double area[])
{
	if(area[0] <= current.posex && current.posex <= area[1] && area[3] <= current.posey && current.posey <= area[2] ){
		exsist = true;
		return true;
	}
	else{
		exsist = false;
		return false;
	}
}
void Human::setPosition(double posX , double posY , double posZ)
{
	static Vec temp= InitVec();
	static int cnt = 0;
	previous = current;
	current.posex = posX;
	current.posey = posY;
	current.posez = posZ;

	if(cnt < 3){
		Vec temp2 = calVec(current , previous);
		temp.vx += temp2.vx;
		temp.vy += temp2.vy;
		cnt++;
	}
	if(cnt == 3){
		movvec.vx = temp.vx / 3.0;
		movvec.vy = temp.vy / 3.0;
		cnt = 0;
		temp = InitVec();
	}
}
void Human::setPosition(double posX,double posY)
{
	static Vec temp= InitVec();
	static int cnt = 0;
	previous = current;
	current.posex = posX;
	current.posey = posY;
	if(RoomPoint[0] >= current.posex || current.posex >= RoomPoint[1] || RoomPoint[3] >= current.posey || current.posey >= RoomPoint[2] ){
		first_enter = true;
	}
	//current.posez = posZ;

	if(cnt < 3){
		Vec temp2 = calVec(current , previous);
		temp.vx += temp2.vx;
		temp.vy += temp2.vy;
		cnt++;
	}
	if(cnt == 3){
		movvec.vx = temp.vx / 3.0;
		movvec.vy = temp.vy / 3.0;
		cnt = 0;
		temp = InitVec();
	}
}
short Human::getActivenessTo(Position target)
{
	Vec vec_to_human = calVec(current,target);
	double theta = calTheta(vec_to_human , movvec);
	double thre_theta = 15.0;
	double movval = calVecVal(movvec);
	data[0] = movval;
	/*double distance = calVecVal(vec_to_human);
	double naiseki = inProduct(vec_to_human , movvec);*/
	if((180.0 - theta) < thre_theta && 10.0 < movval){
		double distance = calDistance(target,current.posex + 10*movvec.vx, current.posex + 10*movvec.vy);
		if(distance < (500.0 * ( 1 + (thre_theta -(180.0 - theta))/thre_theta) * movval)  ){
			will = 1;
			return 1;//near
		}
		else if(distance < (750.0 * ( 1 + (thre_theta -(180.0 - theta))/thre_theta) * movval)  ){
			will = 2;
			return 2;//a little near
		}
		else if(distance < (1000.0 * ( 1 + (thre_theta -(180.0 - theta))/thre_theta) * movval)  ){
			will = 3;
			return 3;//a little far
		}
		else {
			will = 4;
			return 4;//far
		}
	}
	else {
		will = 4;
		return 4;
	}
}


//人の正面に目的地を設定
Position Human::FrontPosition()
{
	double direction = 0.0;
	static double a = 0.0;
	double threshold = 28.0;

	if( (movvec.vx >= threshold || movvec.vx <= -threshold) || (movvec.vy >= threshold || movvec.vy <= -threshold) )
	{
		direction = calTheta(movvec) * PI / 180;
		a = direction;
	}
	else
	{
		direction = a;
	}
	return {current.posex+750*cos(direction), current.posey+750*sin(direction), 0, 0};
}

//人と目的地の間にサブゴールを設定する
Position Human::BetweenPosition(Position target)
{
	Position between;
	double radian = 0.0;

	radian = calDegree(target, current);
	//std::cout << "radian : " << radian << std::endl;

	between.posex = current.posex + 850 * cos((radian * PI / 180) + PI);
	between.posey = current.posey + 850 * sin((radian * PI / 180) + PI);

	return between;
}


//オーナーの背中（ストレンジャーから最も遠い位置）に目的地を設定
Position Human::BehindPosition(Position target, int acceptance)
{
	Position behind;
	double radian = 0.0;
	double distance = 0.0;

	distance = 1000.0 - 5 * acceptance;
	radian = calDegree(target, current);
	behind.posex = current.posex + distance * cos(radian * PI /180);
	behind.posey = current.posey + distance * sin(radian * PI /180);

	return behind;
}

//人の移動速度について
bool Human::movspeed(){
	double threshold = 26.0;

	if( (movvec.vx >= threshold || movvec.vx <= -threshold) || (movvec.vy >= threshold || movvec.vy <= -threshold) ) return true;
	else return false;
}

void Human::disp(){
	std::cout << name <<"\nx : " << current.posex << std::endl;
	std::cout << "y : " << current.posey << std::endl;
	std::cout << "z : " << current.posez << std::endl;
}

float Human::getValue(short type)
{
	if(type == MIS)return miss.value;
	else if(type == ANX) return anxiety.value;
	else return -1;
}

short Human::getState(short type)
{
	if(type == MIS)return miss.state;
	else if(type == ANX) return anxiety.state;
	else return -1;
}
void Human::setOwnerFlag(int flag){
	if(flag == Owner) owner_flag = 0;
	if(flag == SemiOwner) owner_flag = 1;
	if(flag == -1) owner_flag = -1;
}
void Human::setDistanceRobot(double area[], double rx, double ry, double rvx, double rvy){

	double x = 0.0;
	double y = 0.0;
	double prvx = 0.0;
	double prvy = 0.0;
	double theta = 0.0;
	double thetavo = 0.0;
	int count = 0;
	double vx = 0.0;
	double vy = 0.0;
	double t = 0.0;
	static int cnt = 0;
	static int cnt_e = 0;


	/*Position current;
	Position previous;*/
	static Vec temp= InitVec();
	Vec temp2 = InitVec();

	prvx = sqrt(pow(previous.posex - rx, 2));
	prvy = sqrt(pow(previous.posey - ry, 2));

	x = current.posex;
	y = current.posey;

	if(cnt < 3){
		temp2.vx = prvx - sqrt(pow(x - rx, 2));
		temp2.vy = prvy - sqrt(pow(y - ry, 2));
		temp.vx += temp2.vx;
		temp.vy += temp2.vy;
		cnt++;
	}
	if(cnt == 3){
		vx = temp.vx / 3.0;
		vy = temp.vy / 3.0;
		cnt = 0;
		temp = InitVec();
	}

	theta = acos((x - rx ) / sqrt(pow((rx - x), 2) + pow((ry - y), 2)) ) * (180 / 3.1415);
	if(y - ry < 0) theta = 360 - theta;
	t = (rvy ) / sqrt(pow(rvx, 2) + pow(rvy, 2));
	if(t < 0) t = t * (-1);
	thetav = asin( t ) * (180 / 3.1415);
	t = (movvec.vx ) / sqrt(pow((movvec.vx), 2) + pow((movvec.vy), 2));
	if(t < 0) t = t * (-1);
	thetavo = asin( t ) * (180 / 3.1415);

	if( rvx < 0) thetav = 180 - thetav;
	if( rvy < 0) thetav = 360 - thetav;
	if(movvec.vx < 0) thetavo = 180 - thetavo;
	if(movvec.vy < 0) thetavo = 360 - thetavo;

	previous.deg = current.deg ; current.deg = thetavo;
	//pioneer.setHead( thetav);
	thetaor = thetav - theta;
	if(thetav < theta) thetaor *= -1;

	if(theta < 180){
		thetaro = thetavo - (theta + 180);
		if(thetavo < (theta + 180)) thetaro *= -1;
	}
	else{
		thetaro = thetavo - (theta - 180);
		if(thetavo < (theta - 180)) thetaro *= -1;
	}

	if(exsist == true ){
	if(flag_e == 0){
		if(sqrt(pow((rx -current.posex), 2) + pow((ry - current.posey), 2)) < 1000){
				disR = 900;
		//		exsist = true;
			}
			else disR = sqrt(pow((rx - current.posex), 2) + pow((ry - current.posey), 2));
		if(thetaor > 120) disR = 8000;
		//std::cout << thetaor  << "\t" << disR << std::endl;
		}
	else if(flag_e == 1){
		if(attachment < 51) disR = 9000;
		flag_e = 0;
		}
	}
	else {
	disR = 7000;
	flag_e = 1;
	//exsist = false;
	}
	count = 0;
}
void Human::setStressState(){
	//double prex = 0.0;
	//double prey = 0.0;

	if(disR == 7000 ){
		if(exsist == false) stressS = -1000;
		}
		else if(disR == 8000){
			stressS = 0;
		}
		else if(disR < 1000){
			stressS = 100;
		}
		else if(disR == 9000){
			stressS = 1000;
		}
		else{
			lastdis = disR;
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			if((lastdis - disR) > -4000 || (lastdis - disR) < 5000){
				stressS = (lastdis - disR);
				if(stressS > 0) stressS= 0;
				if(stressS <= 0){
					if((thetaor < 270 && thetaor > 90) /*&& (dist(prex, pioneer.getX()) < 50 && dist(prey, pioneer.getY()) < 50)*/){
						stressS = 0;
					}
				}
			}
		}
	//std::cout << exsist << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

void Human::setStress(short type, int typeM, int typeR, double sensitivity, float rate, int ms_d){
	float temp  = 0;

	if(type == MIS)
	{
		temp = miss.value;
		if(typeM == DISTANCE)
		{
			if( disR > 500 * (5000 /  attachment) * (1 / (1 + sensitivity)))
			{
				if(temp - (stressS / 30) + (disR * ms_d * ms_d * rate * sensitivity * attachment) > 100) miss.value = 100;
				else if(temp - (stressS / 30) + (disR * ms_d * ms_d * rate * sensitivity * attachment) < 0) miss.value = 0;
				else miss.value = temp - (stressS / 30) + (disR * ms_d * ms_d * rate * sensitivity * attachment);
			}
		}
		if(typeM == BEHAVIOR)
		{
			if( typeR == DECREASING)
			{
				if(temp - rate < 0)
				{
					miss.value = 0;
					miss.state = STATIC;
				}
				else
				{
					miss.value = temp - rate;
					miss.state = typeR;
				}
			}
			else if(typeR == INCREASING)
			{
				if(temp + rate > 100)
				{
					miss.value = 100;
					miss.state = STATIC;
				}
				else
				{
					miss.value = temp + rate;
					miss.state = typeR;
				}
			}

		}

	}

	if(type == ANX)
	{
		temp = anxiety.value;
		if(typeM == DISTANCE){
			if( disR < 1500 * (1000 /  attachment) * (100 / (1 + sensitivity))){
				if(temp + (stressS / 30) +(( 1 / (disR+1)) * ms_d * ms_d * rate * sensitivity ) > 100) anxiety.value = 100;
				else if(temp + (stressS / 30) + (( 1/ (disR+1)) * ms_d * ms_d * rate * sensitivity) < 0) anxiety.value = 0;
				else anxiety.value = temp + (stressS / 30) + ((1 / (disR+1)) * ms_d * ms_d * rate * sensitivity);
			}
	/*		if( disR > 1500 * (1000 /  attachment) * (1 / (1 + sensitivity))){
				if(temp + (stressS / 30) - (( 1/ (disR+1)) * ms_d * ms_d * rate * sensitivity) > 100) anxiety.value = 100;
				else if(temp + (stressS / 30) - (( 1/ (disR+1)) * ms_d * ms_d * rate * sensitivity ) < 0) anxiety.value = 0;
				else anxiety.value = temp + (stressS / 30) + (( 1 / (disR+1)) * ms_d * ms_d * rate * sensitivity);
			}*/
		}
		if(typeM == BEHAVIOR){
			if( typeR == DECREASING){
				if(temp - rate < 0){
					anxiety.value = 0;
					anxiety.state =STATIC;
				}
				//else if(temp - rate > 100) anxiety.value = 100;
				else{
					anxiety.value = anxiety.value - rate;
					anxiety.state = typeR;
				}

			}
			else if(typeR == INCREASING){
				if(temp + rate > 100){
					anxiety.value = 100;
					anxiety.state =STATIC;
				}
				//else if(temp - rate < 0) anxiety.value = 0;
				else{
					anxiety.value = anxiety.value + rate;
					anxiety.state = typeR;
				}
			}
		}
	}
}

Human::~Human()
{
}
