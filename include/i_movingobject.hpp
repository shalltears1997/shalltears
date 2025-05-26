#pragma once

#include <iostream>
#include <thread>
#include <c_struct_position.hpp>
//#include <tk_data_struct.hpp>


class MovingObjectClass
{
private:

	 DATA_object p_object;
	 double p_monitoring_necessity = 0.0;
	 bool necessity = false;
	 bool blind = true;
	 bool owner_flag = false;
	 DATA_xy pre_coordinate = {};
	 double speed_obj = 0.0;
	 int emergency_point = 0;

public:

	 // コンストラクタ
  MovingObjectClass(){};
  void initialize_data(int num);
  double change_max(double max);
  bool getnecessity(){return necessity;};
  void setnecessity();
  void setblind();
  bool getblind(){return blind;};
  int recog_inmap(int num);
  DATA_grid grid_object();
  void owner_necessity(DATA_grid owner);
  void recog_owner();
  bool getownerflag(){return owner_flag;};


};
