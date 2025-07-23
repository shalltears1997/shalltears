#pragma once

#include "tk_data_struct.hpp"
#include "class.hpp"
#include <chrono>

class Vocalization
{

private:
	double flag = 0.0;

	double robot_potential = 0.0;

	double vocal_pote = 0.0;

	//double c_a = 0.0;
	
	std::chrono::system_clock::time_point time_first;
	std::chrono::system_clock::time_point time_next;

public:

	// コンストラクタ
	Vocalization(){};

	//double activation(DATA_xy robot);
	double activation(DATA_xy robot, double miss, double anxeity, double moving_stress);

};
