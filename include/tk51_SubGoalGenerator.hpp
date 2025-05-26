#pragma once

#include <vector>

#include "c_struct_position.hpp"
#include "l_tk-library.hpp"


class SubGoalGenerator
{
private:
  std::vector<DATA_xy> p_route_point;

public:
  SubGoalGenerator(std::vector<DATA_xy> p_route_point_in);
  DATA_xy generateSubGoal(double speed_in);
};
