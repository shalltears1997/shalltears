#pragma once

#include <iostream>
#include <thread>
#include <c_struct_position.hpp>



class CalWeightClass
{
private:

	 float p_weight[60][30] = {};
	 float p_weight_area[6][3] = {};
	 float N_weight_area[6][3] = {};
	 int blind[6][3] = {{1,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}};




public:

	 // コンストラクタ
	 CalWeightClass(){};
	 void initialize_weight();
	 void weight_area();
	 void normalize_weight();
	 DATA_grid max_weight();
	 DATA_grid min_weight();
	 int getblind(int yoko,int tate);


};
