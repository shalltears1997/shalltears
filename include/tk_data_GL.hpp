#pragma once

#include <vector>

#include "define.hpp"
#include "c_struct_position.hpp"
#include "tk_data_struct.hpp"
#include "l_tk_pote.hpp"
#include "tk_data_class.hpp"


#include "opengl.hpp"
class GLDataClass
{
public:



  // 内部パラメータ
  int distype = 0;
  int potentialFlag = 0;//tuika
  bool stranger_ballflag = false;
  int playcount = 0;
  bool mouseFlagR = false;
  unsigned char mouseFlag = GL_FALSE;
  int drag_flag = 0;
  int display_switch = 1;

  // バッファ（入力用）
  // bool GoToHomeFlag = false;
  // bool nazo_expflag = false;
  // bool pioneer_activate = false;
  // int human_num = 3;
  // std::vector<Position> po_human;
  // PARAMETER pa_human[3];
  // double sensitivity_of_anxiety = 0;

  DataClass data;

  // GLDataClass()
  // {
  //   std::vector<Position> tmp(human_num);
  //   po_human = tmp;
  // }

  void dataInput(DataClass *data_in);

};
