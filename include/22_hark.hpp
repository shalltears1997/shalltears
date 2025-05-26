#pragma once

//	音源定位（2016/05/25、塚田）
struct hark_struct
{
	int count;
	int size[5];	//int num;
	double power[5];
	double angle[5];
	double power_ave[5];
	double angle_ave[5];
	double sd[5];
	bool flag;
	double angle_gap;
};
