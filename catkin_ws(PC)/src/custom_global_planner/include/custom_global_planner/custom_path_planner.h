// 【修改】新增 TF/裁剪相关参数声明，并与 costmap/tf2 头文件对齐
#ifndef CUSTOM_PATH_PLANNER_H_
#define CUSTOM_PATH_PLANNER_H_

#include <nav_core/base_global_planner.h>
#include <ros/ros.h>
#include <nav_msgs/Path.h>
#include <geometry_msgs/PoseStamped.h>
#include <vector>

// ★新增
#include <costmap_2d/costmap_2d_ros.h>
#include <costmap_2d/costmap_2d.h>
#include <tf2_ros/transform_listener.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>

namespace custom_global_planner {

class CustomPathPlanner : public nav_core::BaseGlobalPlanner
{
public:
  CustomPathPlanner();
  CustomPathPlanner(std::string name, costmap_2d::Costmap2DROS* costmap_ros) { initialize(name, costmap_ros); }
  virtual void initialize(std::string name, costmap_2d::Costmap2DROS* costmap_ros) override;
  virtual bool makePlan(const geometry_msgs::PoseStamped& start,
                        const geometry_msgs::PoseStamped& goal,
                        std::vector<geometry_msgs::PoseStamped>& plan) override;

private:
  void pathCb(const nav_msgs::Path& msg);

  // 状态
  bool initialized_ = false;
  ros::NodeHandle nh_, pnh_;
  ros::Subscriber path_sub_;
  costmap_2d::Costmap2DROS* costmap_ros_ = nullptr;
  std::string global_frame_ = "map";
  std::string path_topic_   = "/custom_global_path";

  tf2_ros::Buffer tf_buffer_;
  tf2_ros::TransformListener tf_listener_{tf_buffer_};

  std::vector<geometry_msgs::PoseStamped> cached_plan_;
  ros::Time last_served_{0};
  ros::Time last_path_stamp_{0};

  // 参数
  double min_period_       = 0.5;   // makePlan 最小间隔(s)
  int    downsample_step_  = 3;     // 路径降采样步长(>=1)
  bool   filter_outside_   = true;  // 过滤代价图外的点

  // ★新增：从最近点裁剪、TF 查表超时、是否使用 0 时刻
  bool   clip_from_nearest_ = true;   // ★新增
  double tf_lookup_timeout_ = 0.1;    // ★新增
  bool   tf_use_zero_time_  = true;   // ★新增
};

}  // namespace custom_global_planner

#endif  // CUSTOM_PATH_PLANNER_H_

