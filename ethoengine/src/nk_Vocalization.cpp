#include <iostream>
#include <cmath>
#include <ctime>
#include <termios.h>
#include "nk_Vocalization.hpp"
#include "class.hpp"
#include "gv_extern.hpp"

double Vocalization::activation(DATA_xy robot, double miss, double anxiety, double moving_stress)
{
	double d_min = calDistance(robot, min_pote);
	double d_max = calDistance(robot, max_pote);

	double robot_potential = data.dMiss_score[double2grid_x(robot.x)][double2grid_y(robot.y)];

	double c_a =  data.dAnxiety_score[double2grid_x(robot.x)][double2grid_y(robot.y)] * -max_stress_potential;

	double c_m = (robot_potential / max_stress_potential)*1.5 + (robot_potential / min_stress_potential)*3;

	//print_delta = (robot_potential / max_stress_potential)*1.5;
	//print_delta2 = (robot_potential / min_stress_potential)*3;

	if(c_m < -20)
	{
		c_m = -20;
	}
	else if(c_m > 2)
	{
		c_m = 2;
	}
	
	if(c_a > 2)
	{
		c_a = 2;
	}
	else if(c_a < -200)
	{
		c_a = -200;
	}

	//print_delta2 = c_a;

	if(c_a < 0)
	{
		time_first = std::chrono::system_clock::now();
	}

	time_next = std::chrono::system_clock::now();
	double t = (double)(std::chrono::duration_cast<std::chrono::microseconds>(time_next - time_first).count()) / 1000.0;

	//print_delta2 = 10 * exp(-t/1000);

	double c2 = 10 * exp(-t/1000);

	vocal_pote = (c_m * (miss / (miss + anxiety + 1))+0.5) + c_a * (anxiety / (miss + anxiety + 1));

	if(c2 > 1)
	{
		//vocal_pote = -100;
		vocal_pote = vocal_pote * -10 * c2;
	}

	if(vocal_pote < -200)
	{
		vocal_pote = -200;
	}
	else if(vocal_pote > 2)
	{
		vocal_pote = 0;
	}

	if(vocal_pote < -1 && vocal_pote >= -20)
	{
		print_delta2 = 1400 + vocal_pote * 10;
	}
	else if(vocal_pote < -20)
	{
		print_delta2 = 700 + vocal_pote;
	}
	else if(vocal_pote >= 1)
	{
		print_delta2 = 1200 + vocal_pote * 100;
	}
	else
	{
		print_delta2 = 0;
	}


	//print_delta2 = vocal_pote;

	//return flag;
	return vocal_pote;
}
