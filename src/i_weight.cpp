#pragma once

#include <iostream>
#include <cstring>
#include <chrono>
#include <thread>
#include <cmath>
#include <cstdio>

#include "gv_extern.hpp"
#include <sys/select.h>
#include "i_weight.hpp"

void CalWeightClass::initialize_weight()
{
	for(int i=0;i<60;i++)
	{
		for(int j=0;j<30;j++)
		{
			p_weight[i][j] = data.weight[i][j];
		}
	}

}

void CalWeightClass::weight_area()
{
	for(int i=0;i<6;i++)
	{
		for(int j=0;j<3;j++)
		{
			p_weight_area[i][j] = 0.0;

			for(int k=0;k<10;k++)
			{
				for(int l=0;l<10;l++)
				{
					p_weight_area[i][j] += data.weight[i*10 + k][j*10 + l];
					//data.weigth_area[i][j] = p_weight_area[i][j];
				}
			}
		}
	}


}

DATA_grid CalWeightClass::max_weight()
{
	float imax = -10.0;
	DATA_grid max_grid;

	for(int i=0;i<6;i++)
	{
		for(int j=0;j<3;j++)
		{
			if(p_weight_area[i][j] > imax)
			{
				imax = p_weight_area[i][j];
				max_grid.i = i;
				max_grid.j = j;
			}
		}
	}

	return max_grid;
}

DATA_grid CalWeightClass::min_weight()
{
	float imin = 99999999.0;
	DATA_grid min_grid;

	for(int i=0;i<6;i++)
	{
		for(int j=0;j<3;j++)
		{
			if(p_weight_area[i][j] < imin)
			{
				imin = p_weight_area[i][j];
				min_grid.i = i;
				min_grid.j = j;
			}
		}
	}
	return min_grid;
}

void CalWeightClass::normalize_weight()
{
	float Y = 0.0;

	for(int i=0;i<6;i++)
	{
		for(int j=0;j<3;j++)
		{
			N_weight_area[i][j] = (p_weight_area[i][j] - p_weight_area[min_weight().i][min_weight().j])/((p_weight_area[max_weight().i][max_weight().j]-p_weight_area[min_weight().i][min_weight().j])*(1-0.1)+0.1);
			data.weigth_area[i][j] = N_weight_area[i][j];
		}

	}
}

int CalWeightClass::getblind(int yoko,int tate)
{
	return blind[yoko][tate];
}
