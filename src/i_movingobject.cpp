#include <iostream>
#include <cstring>
#include <chrono>
#include <thread>
#include <cmath>
#include <cstdio>

#include "l_tk-library.hpp"
#include "gv_extern.hpp"

#include <sys/select.h>

void MovingObjectClass::initialize_data(int num)
{
	p_object = data.object[num];
}

double MovingObjectClass::change_max(double max)
{
	int i,j;

	if(p_object.monitoring_necessity > max)
	{
		max = p_object.monitoring_necessity;
	}

	return max;
}

void MovingObjectClass::setnecessity()
{
	//PRINT("%d,%lf",p_object.oID,p_object.monitoring_necessity);
	if(p_object.monitoring_necessity > 0.7)
	{
		necessity = true;
	}
	else if(p_object.monitoring_necessity <= 0.70 && necessity == true)
	{
		if(p_object.monitoring_necessity < 0.1)necessity = false;
	}
	else necessity = false;
}

DATA_grid MovingObjectClass::grid_object()
{

	DATA_grid object_area = {};

	object_area.i = (int)((p_object.posi.x + 6000/2)/(6000/6));
	object_area.j = (int)((3000 - p_object.posi.y)/(3000/3));

	return object_area;

}

int MovingObjectClass::recog_inmap(int num)
{
	if((p_object.posi.x < 3000.0 && p_object.posi.x > -3000.0 && p_object.posi.y > 0.0 && p_object.posi.y < 3000.0 )||(p_object.posi.x<3000 &&p_object.posi.x>2500 && p_object.posi.y>3000 && p_object.posi.y <4000))
	{
		p_object.inmap =  1;
	}
	else
	{
		p_object.inmap = 0;
		p_object.monitoring_necessity=0.0;
				data.object[num] = p_object;
	}

	return p_object.inmap;
}

void MovingObjectClass::setblind()
{
	/*double deg1=0;
	double deg2=0;

	deg1 = calDegree(data.wall_edge,p_object.posi);
	deg2 = calDegree(data.human[0].posi,data.wall_edge);

	PRINT("deg1:%lf\n",deg1);
	PRINT("deg2:%lf\n",deg2);

	/*	DATA_xy vecA = {};
	DATA_xy vecB = {};
	double A =0;
	double B =0;

	double sita =0;

	//壁→動物体のベクトル
	vecA.x = (p_object.posi.x-data.wall_edge.x);
	vecA.y = (p_object.posi.y-data.wall_edge.y);
	//壁→人
	vecB.x = (data.human[0].posi.x-data.wall_edge.x);
	vecB.y = (data.human[0].posi.y-data.wall_edge.y);

	sita = */

	if(calDistance(data.human[0].posi,p_object.posi) < 900 || calDegree(data.human[0].posi,p_object.posi) < 120 ||judge_in_ellipse(data.human[0].posi.x,data.human[0].posi.y))
	{
		blind = false;
	}
	else blind = true;
}

void MovingObjectClass::owner_necessity(DATA_grid owner)//一定時間たったら緊急事態を検出する処理
{

	//PRINT("point:%d\n",emergency_point);

	if(exist_judge_human(data.human[0].posi))
	{
	speed_obj = calDistance((data.human[0].posi.x - pre_coordinate.x),(data.human[0].posi.y - pre_coordinate.y));

	if(speed_obj < 50)
	{
		emergency_point++;
	}
	else if(speed_obj >=50 )
	{
		emergency_point = 0;
		data.human[0].monitoring_necessity = 0.0;

	}

	if(exist_judge_robot(data.human[1].posi))
	{
		emergency_point = 0;
			data.human[0].monitoring_necessity = 0.0;
	}

	if(emergency_point > 600/(/*data.weigth_area[owner.i][owner.j]+*/1.0))
	{
		data.human[0].monitoring_necessity = 1.0;
	}
	pre_coordinate = data.human[0].posi;
	}
	else
	{
		emergency_point = 0;
	}
}

void MovingObjectClass::recog_owner()
{
	if(data.human[0].posi.x-500 < p_object.posi.x && p_object.posi.x <data.human[0].posi.y+500 && data.human[0].posi.y-500 < p_object.posi.y && p_object.posi.y <data.human[0].posi.y+500)
	{
		owner_flag = true;
	}
	else owner_flag = false;
}
