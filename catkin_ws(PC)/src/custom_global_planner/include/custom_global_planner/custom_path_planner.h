#ifndef CUSTOM_PATH_PLANNER_H_
#define CUSTOM_PATH_PLANNER_H_

#include <nav_core/base_global_planner.h>
#include <ros/ros.h>
#include <nav_msgs/Path.h>
#include <geometry_msgs/PoseStamped.h>
#include <vector>

namespace custom_global_planner {

class CustomPathPlanner : public nav_core::BaseGlobalPlanner
{
public:
  CustomPathPlanner();
  CustomPathPlanner(std::string name, costmap_2d::Costmap2DROS* costmap_ros);
  virtual void initialize(std::string name, costmap_2d::Costmap2DROS* costmap_ros);
  virtual bool makePlan(const geometry_msgs::PoseStamped& start,
                        const geometry_msgs::PoseStamped& goal,
                        std::vector<geometry_msgs::PoseStamped>& plan);

private:
  void pathCallback(const nav_msgs::Path::ConstPtr& msg);

  ros::NodeHandle nh_;
  ros::Subscriber path_sub_;
  std::vector<geometry_msgs::PoseStamped> cached_path_;
  bool initialized_;
};

}  // namespace custom_global_planner

#endif  // CUSTOM_PATH_PLANNER_H_
