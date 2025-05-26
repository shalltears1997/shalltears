#pragma once

#include "define.hpp"

class Human// : public PotentialGenerator
{
private:
public:
	double attachment;
	int humanID;
	char name[16];
	clock_t timer;
	Position current;
	Position previous;
	Vec movvec;
	short will;
	double data[4];
	bool first_enter;
	InnerState miss;
	InnerState anxiety;
	int owner_flag;
	double disR;
	double lastdis;
	double thetav;
	double thetaor;
	double thetaro;
	double stressS;
	bool exsist;
	int flag_e;
// public:
	Human(double value , char type[]);
	//bool Exist();
	bool exist(double area[]);
	bool isFirst(){return first_enter;};
	void isKnown(){first_enter = false;};
	void setPosition(double posX,double posY,double posZ);
	void setPosition(double posX,double posY);
	void setDeg(double degree){previous.deg = current.deg ; current.deg = degree;};
	Position getCurrentPosition(){return current;};

	Position FrontPosition();
	Position BetweenPosition(Position target);
	Position BehindPosition(Position target, int acceptance);
//	bool Human::movspeed();//soh追加
	bool movspeed();//soh追加

	//Position getPreviousPosition(){return previous;};
	void setAttachment(double value){if(0 <= value && value <= 100)attachment = value;};
	void disp();
	void draw();
	double getX(){return current.posex;};
	double getY(){return current.posey;};
	double getZ(){return current.posez;};
	double getDeg(){return current.deg;};
	double getvecX(){return movvec.vx;};
	double getvecY(){return movvec.vy;};
	short getActivenessTo(Position target);
	double getAttachment(){return attachment;};
	void setMiss(double Value,short State){miss.value= Value; miss.state = State;};
	void setAnxiety(double Value,short State){anxiety.value = Value; anxiety.state = State;};
	InnerState getMiss(){return miss;};
	InnerState getAnxiety(){return anxiety;};
	void drawInnerState(int i);
	float getValue(short type);
	short getState(short type);
	int getOwnerFlag(){return owner_flag;};
	void setOwnerFlag(int flag);
	void setDistanceRobot(double area[], double rx, double ry, double rvx, double rvy);
	double getDistanceRobot(){return disR;};
	void setStressState();
	double getStressState(){return stressS;};
	void setStress(short type, int typeM, int typeR, double sensitivity, float rate, int ms_d);
	~Human();

};
