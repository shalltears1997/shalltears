#include <iostream>
#include <cstdlib>
#include <cmath>
#include <algorithm>  // vectorのソートに使う
#include <tuple>  // 複数の戻り値

#include "opengl.hpp"

#include "define.hpp"
#include "gv_extern_id.hpp"
#include "c_struct_position.hpp"
#include "l_tk_pote.hpp"
#include "l_tk-library.hpp"

#include "tk_data_class.hpp"

#include "tk_data_struct.hpp"

////////////////////////////////////////////////////////////////////////////////
// Function - 内部メソッド
////////////////////////////////////////////////////////////////////////////////

DATA_object po_potedama;

// 斥力（壁際のバリア）
double PotentialFunction::calBarrier(double x, double y, double approach_distance)
{
	double value = 0;

	double distance;
	double margin = 0.7;

	distance = abs(external_region_xXyY[0]-x)/1000;
	// distance = abs(external_region_xXyY[0]+1000-x)/1000;
	if(distance < margin) value += pow(approach_distance, 3) / pow(distance, 2);

	distance = abs(external_region_xXyY[1]-x)/1000;
	if(distance < margin) value += pow(approach_distance, 3) / pow(distance, 2);

	distance = abs(external_region_xXyY[2]-y)/1000;
	if(distance < margin) value += pow(approach_distance, 3) / pow(distance, 2);

	distance = abs(external_region_xXyY[3]-y)/1000;
	if(distance < margin) value += pow(approach_distance, 3) / pow(distance, 2);


	return value;
}

// アタッチメントと接近距離の関係
double PotentialFunction::attachment2distance(int attachment)
{
	double d_max = 1250, d_min = 750;
	int a_max = 100;
	double distance = (double)(a_max - attachment)/a_max * (d_max - d_min) + d_min;
	return distance/1000;
}

// 目的地（傾き一定）
double PotentialFunction::calSlope(double x, double y, DATA_xy origin)
{
	double distance = calDistance(origin.x-x, origin.y-y)/1000;
	return 2 * distance;
}

// 斥力（人）
double PotentialFunction::calMountain(double x, double y, DATA_xy origin, double approach_distance)
{
	double distance = calDistance((origin.x-x), (origin.y-y))/1000;
	return pow(approach_distance, 3) / pow(distance, 2);
}

// 障害物
double PotentialFunction::calObstacle(double x, double y, DATA_obstacle obstacle)
{
	if(double2grid_x(obstacle.x0) <= double2grid_x(x) && double2grid_x(x) <= double2grid_x(obstacle.x1)
			&& double2grid_y(obstacle.y0) <= double2grid_y(y) && double2grid_y(y) <= double2grid_y(obstacle.y1))
	{
		return INFINITY;
	}
	else
	{
		return 0;
	}
}

////////////////////////////////////////////////////////////////////////////////
// Function - 値参照
////////////////////////////////////////////////////////////////////////////////

// ある一点のポテンシャル値算出
double PotentialFunction::potential(double x, double y)
{
	double potential = 0;

	// if( -3000<x && x<3000 && 0<y && y<3000 )
	if( external_region_xXyY[0]<x && x<external_region_xXyY[1] && external_region_xXyY[2]<y && y<external_region_xXyY[3] )
	{
		potential += calBarrier(x, y, 20);
		if(condition) potential += calSlope(x, y, p_targetpose);  // 目的地
		potential += calMountain(x, y, po_owner, attachment2distance(pa_owner));  // 人
		potential += calMountain(x, y, po_stranger, attachment2distance(pa_stranger));  // 人

		//potential += calMountain(x, y, po_potedama.posi, attachment2distance(pa_stranger));  // ポテ玉

		potential += calMountain(x,y,{-1500,2550},attachment2distance(10));

		//potential += calMountain(x, y, {3000, 2700}, attachment2distance(80));  // 壁（人と同じ）

		// potential += calMountain(x, y, {2000, 1000}, attachment2distance(100));  // 椅子（人と同じ）
		// potential += calMountain(x, y, {750, 2000}, attachment2distance(100));  // 椅子（人と同じ）
		//for(int i=0; i<10; i++) potential += calObstacle(x, y, p_obstacle[i]);  // 障害物
	}
	else
	{
		potential = INFINITY;
	}

	return potential;
}

// ある一点のポテンシャル値算出
double PotentialFunction::potential_grid(int i, int j)
{
	return potential(grid2double_x(i), grid2double_y(j));
}

////////////////////////////////////////////////////////////////////////////////
// Generator
////////////////////////////////////////////////////////////////////////////////

// コンストラクタ

PotentialGenerator::PotentialGenerator()
{
	//
}
PotentialGenerator::PotentialGenerator(std::vector<DATA_human> human, PARAMETER human_p[], DATA_obstacle obstacle[])
{
	setParm(human, human_p, obstacle);
}
PotentialGenerator::PotentialGenerator(DATA_xy targetpose, std::vector<DATA_human> human, PARAMETER human_p[], DATA_obstacle obstacle[])
{
	setParm(targetpose, human, human_p, obstacle);
}

// 目的地なしセット
void PotentialGenerator::setParm(std::vector<DATA_human> human, PARAMETER human_p[], DATA_obstacle obstacle[])
{
	p_targetpose = {0,0};
	po_owner = human[0].posi;
	po_stranger = human[1].posi;
	pa_owner = human_p[0].ata;
	pa_stranger = human_p[1].ata;
	p_obstacle = obstacle;
	condition = false;
}

// 目的地ありセット
void PotentialGenerator::setParm(DATA_xy targetpose, std::vector<DATA_human> human, PARAMETER human_p[], DATA_obstacle obstacle[])
{
	p_targetpose = targetpose;
	po_owner = human[0].posi;
	po_stranger = human[1].posi;
	pa_owner = human_p[0].ata;
	pa_stranger = human_p[1].ata;
	p_obstacle = obstacle;
	condition = true;
}


// 全グリッドのポテンシャル値算出
std::vector<std::vector<double>> PotentialGenerator::getPoteGridMap()
{
	std::vector<std::vector<double>> pote(gridnum1, std::vector<double>(gridnum2, 0));

	for(int i = 0; i < gridnum1; i++)
	{
		for(int j = 0; j < gridnum2; j++)
		{
			pote[i][j] = potential(grid2double_x(i), grid2double_y(j));
		}
	}

	return pote;
}


// 最低値の算出
DATA_grid_part PotentialGenerator::getlowestGrid()
{
	DATA_grid_part lowest_grid = {{0,0}, INFINITY};

	for(int i = 0; i < gridnum1; i++)
	{
		for(int j = 0; j < gridnum2; j++)
		{
			double value = potential(grid2double_x(i), grid2double_y(j));
			if(value < lowest_grid.value)
			{
				lowest_grid.value = value;
				lowest_grid.grid.i = i;
				lowest_grid.grid.j = j;
			}
		}
	}
	return lowest_grid;
}

// 最低値群の算出
std::vector<DATA_grid_part> PotentialGenerator::getLowGrids()
{
	// 最小値を見つけて配列の一番目の要素にする
	std::vector<DATA_grid_part> low_grids(1, getlowestGrid());

	// 最小値に近い値をもつ最小値群を加える
	for(int i = 0; i < gridnum1; i++)
	{
		for(int j = 0; j < gridnum2; j++)
		{
			double value = potential_grid(i, j);
			if( value - low_grids[0].value < 0.05 )
			{
				low_grids.push_back({{i, j}, value});
			}
		}
	}
	return low_grids;
}
