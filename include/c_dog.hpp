#pragma once

#include "define.hpp"
#include "tk_data_class.hpp"

class Dog
{
// private:
public:
	Position current;
	Position previous;

	//内部パラメータ
	//owner
	InnerState miss;
	InnerState o_anxiety;
	InnerState o_explore;
	//stranger
	InnerState s_miss;
	InnerState anxiety;
	InnerState s_explore;
	//stranger2
	InnerState s2_miss;
	InnerState s2_anxiety;
	InnerState s2_explore;
	//ball
	InnerState b_miss;
	InnerState b_anxiety;
	InnerState b_explore;
	//oyatsu
	InnerState oy_miss;
	InnerState oy_anxiety;
	InnerState oy_explore;
	//area
	InnerState explore;
	Vec movvec;

//頭部
	double tilt_deg;//峻治追加
	double pan_deg;//峻治追加
	double headtime;

	//既知未知領域
	float known_level[area1][area2];
	//選択された行動ID
	int motionID;
	bool willingness;
	double sensitivity;
	bool movingflag;
public:
	Dog();
	void draw(double RED, double GREEN, double BLUE, double radius , clock_t time, int ROBOT, double dis);
	void draw2(double RED, double GREEN, double BLUE, double radius , clock_t time);
	void setPosition(double posX,double posY, double degree);
	void setPosition(double posX,double posY);
	void setPosition(double posX , double posY,int simu);



	// 現在角度
	double trans_body_deg;
	void setTransBodyDeg(double degree);
	double getTransBodyDeg();

	// 目標角度
	double final_body_deg;
	void setFinalBodyDeg(double degree);
	void setFinalBodyDeg(Position target);
	double getFinalBodyDeg();

	// 描画角度（スレッドで回している間の仮）
	double draw_body_deg;
	void setDrawBodyDeg(double degree);




	// tk1701
	void setFinalDeg(Position target);
	void setGoalPosition(Position setposition);
	void rotateSimRobot();
	// tk1701
	double final_deg;
	Position goal;
	double getFinalDeg(){return final_deg;};
	double robot_angle;
	void setRobotDeg(double degree);


	//頭部
	//void Head(double degree);//峻治追加s
	void Head(Position target);//峻治追加tk
	void Head(Position target , Position current);//峻治追加
	void Head(Position target, Position current, double attachment);
	double getPan_deg(){return pan_deg;};//峻治
	double getTilt_deg(){return tilt_deg;};//峻治
	void HeadTime(double attachment);
	double getHeadTime(){return headtime;};//峻治
	void attachment_Head(double attachment);
	void Head(Position target, Position current, int motionID, double attachment);
	void Head(Position target, int motionID);

	void setMotion(int m_ID){motionID = m_ID;};
	int getMotionID(){return motionID;};
	Position getCurrnetPosition(){return current;};
	Position getPreviousPosition(){return previous;};
	double getX(){return current.posex;};
	double getY(){return current.posey;};
	double getvecX(){return movvec.vx;};
	double getvecY(){return movvec.vy;};
//	double getDeg(){return current.deg;};	// tk1701
	double getDeg(){return robot_angle;};	// tk1701

	void setMiss(double Value,short State){miss.value= Value; miss.state = State;};
	void setO_Anxiety(double Value,short State){o_anxiety.value = Value; o_anxiety.state = State;};
	void setO_Explore(double Value,short State){o_explore.value = Value; o_explore.state = State;};

	void setS1_Miss(double Value,short State){s_miss.value= Value; s_miss.state = State;};
	void setAnxiety(double Value,short State){anxiety.value = Value; anxiety.state = State;};
	void setS1_Explore(double Value,short State){s_explore.value = Value; s_explore.state = State;};

	void setS2_Miss(double Value,short State){s2_miss.value= Value; s2_miss.state = State;};
	void setS2_Anxiety(double Value,short State){s2_anxiety.value = Value; s2_anxiety.state = State;};
	void setS2_Explore(double Value,short State){s2_explore.value = Value; s2_explore.state = State;};

	void setB_Miss(double Value,short State){b_miss.value= Value; b_miss.state = State;};
	void setB_Anxiety(double Value,short State){b_anxiety.value = Value; b_anxiety.state = State;};
	void setB_Explore(double Value,short State){b_explore.value = Value; b_explore.state = State;};

	void setOY_Miss(double Value,short State){oy_miss.value= Value; oy_miss.state = State;};
	void setOY_Anxiety(double Value,short State){oy_anxiety.value = Value; oy_anxiety.state = State;};
	void setOY_Explore(double Value,short State){oy_explore.value = Value; oy_explore.state = State;};

	void setExplore(double Value,short State){explore.value = Value; explore.state = State;};

	void setSens(double Value){sensitivity = Value;};
	double getSens(){return sensitivity;};
	void setAgreeCnt(int a_cnt,int u_cnt);
	bool getWillingness(){return willingness;};
	InnerState getMiss(){return miss;};
	InnerState getAnxiety(){return anxiety;};
	InnerState getExplore(){return explore;};
	float getValue(short type);
	short getState(short type);
	void moving(bool true_or_false){movingflag = true_or_false;};
	bool isMoving(){return movingflag;};
	void disp();
	bool updateKnownLevel();
	bool decreaseKnownLevel();
	float getKnownLevel(int inum , int jnum){return known_level[inum][jnum];};
	int getNumberOfKnownPlace();
	void drawKnownArea();
	void drawInnerState();
	void drawMode();
	void drawMode2();
	bool exist(double area[]);
};
