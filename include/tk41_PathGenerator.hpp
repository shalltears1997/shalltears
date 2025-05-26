#pragma once

#include <vector>

#include "c_struct_position.hpp"
#include "l_tk-library.hpp"


class PathGenerator
{
private:
  bool valley_judge(double x, double y);
  bool new_method(double x, double y, DATA_xy *value);
  bool goal_reach_judge(DATA_xy point);

  void potential_barrier();

  // 入力
  PotentialFunction p_ptf;
  DATA_xy p_robot;

  // 出力
  int p_tki_length = tki_MAX;
  // DATA_xy p_route_point[tki_MAX];

  std::vector<DATA_xy> p_route_point;
  std::vector<DATA_grid_part> low_grids;

public:
  PathGenerator(PotentialFunction ptf_in, std::vector<DATA_grid_part> low_grids_in, DATA_xy robot);

  std::vector<DATA_xy> generate_path();
  bool potential_sim();

};
