#pragma once

#include "tk32_BasicBehaviors.hpp"

class Component_ActivateBehavior
{
private:
  DataClass *data, *data_pre;
  // DebugDataClass debug_data;
  // ConfigDataClass config_data;
  BehaviorClass *behavior_p;

  void RE_ConstructBehavior();

public:
	Component_ActivateBehavior(DataClass *data_in, DataClass *data_pre_in);
  void act();
};
