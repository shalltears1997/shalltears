#include "tk_data_struct.hpp"
#include "class.hpp"

class BehaviorSelectorClass
{
public:
  BehaviorSelectorClass(){};
  // int select(int now, Dog pioneer, PARAMETER owner, PARAMETER stranger, PARAMETER unfamilier, BALL_yoheisan ball_yo, BALL ball_st, bool sound_move, ConfigDataClass config_data_in, NAZO nazo);
  int select(std::vector<DATA_human> po_human, PARAMETER pa_human[], BALL_yoheisan ball_yo, BALL ball_st, bool sound_move, ConfigDataClass config_data_in, NAZO nazo);

  double attachment_filter(double attachment);

  double robot_velocity(DATA_xy subgoal, DATA_xy robotposition, double sensitivity_of_anxiety);
};
