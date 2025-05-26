#pragma once

#include <ctime>

class Room
{
private:
//	clock_t start, end;
	clock_t start;
	clock_t end;
	double Width;
	double Hight;

public:
	Room();
	//char display_explore[32],display_anxiety[32],display_miss[32],display_time[32],display_place[32],display_mode[32],display_ato[32],display_aos[32],display_soa[32];
	void drawBackground();
	void drawRoom();
	void drawNoEnterArea();
	void drawNoSleepArea();
	void drawParameterBar(short type, float value);
	void drawTime();
	void drawKnownPlace(int number_of_place);
	void drawMode();
	void drawParameter(double value1 ,double value2 , double value3 , double value4);
	clock_t getTime(){return start;};
	//double* GetNoEnter(){return NoEnter;};
	//double* GetNoSleep(){return NoSleep;};
	//double* GetRoom(){return RoomPoint;};
	//void explore_area(int point);
	void drawGrid(int num1,int num2);
	~Room();
};
