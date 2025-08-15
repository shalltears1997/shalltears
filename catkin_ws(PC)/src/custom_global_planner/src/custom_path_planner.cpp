#include "custom_global_planner/custom_path_planner.h"
#include <pluginlib/class_list_macros.h>

namespace custom_global_planner {

CustomPathPlanner::CustomPathPlanner() : initialized_(false) {}

CustomPathPlanner::CustomPathPlanner(std::string name,
                                     costmap_2d::Costmap2DROS* costmap_ros)
{
  initialize(name, costmap_ros);
}

void CustomPathPlanner::initialize(std::string name,
                                   costmap_2d::Costmap2DROS* costmap_ros)
{
  if (!initialized_) {
    // 订阅自定义路径话题
    path_sub_ = nh_.subscribe("/custom_global_path", 1,
                              &CustomPathPlanner::pathCallback, this);
    ROS_INFO("CustomPathPlanner initialized");
    initialized_ = true;
  }
}

void CustomPathPlanner::pathCallback(const nav_msgs::Path::ConstPtr& msg)
{
  cached_path_.clear();
  // 拷贝所有 PoseStamped
  for (const auto& pose : msg->poses) {
    cached_path_.push_back(pose);
  }
}

bool CustomPathPlanner::makePlan(const geometry_msgs::PoseStamped& start,
                                 const geometry_msgs::PoseStamped& goal,
                                 std::vector<geometry_msgs::PoseStamped>& plan)
{
  plan.clear();
  if (!initialized_) {
    ROS_ERROR("CustomPathPlanner has not been initialized");
    return false;
  }

  if (cached_path_.empty()) {
    ROS_WARN("CustomPathPlanner: cached path is empty");
    return false;
  }

  // 将缓存的路径作为全局路径返回
  for (const auto& p : cached_path_) {
    plan.push_back(p);
  }
  return true;
}

}  // namespace custom_global_planner

// 导出插件
PLUGINLIB_EXPORT_CLASS(custom_global_planner::CustomPathPlanner, nav_core::BaseGlobalPlanner)
