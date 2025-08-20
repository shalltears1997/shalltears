// 【修改】统一 TF 为 zero time；路径统一到 map + stamp=0；支持最近点裁剪；越界过滤
#include "custom_global_planner/custom_path_planner.h"
#include <pluginlib/class_list_macros.h>
#include <algorithm>
#include <limits>

namespace custom_global_planner {

CustomPathPlanner::CustomPathPlanner() : nh_(), pnh_("~") {}

void CustomPathPlanner::initialize(std::string name, costmap_2d::Costmap2DROS* costmap_ros)
{
  if (initialized_) return;

  costmap_ros_  = costmap_ros;
  global_frame_ = costmap_ros_->getGlobalFrameID();

  // 私有命名空间（/move_base/<name>/...）
  ros::NodeHandle private_nh("~/" + name);
  private_nh.param("path_topic",      path_topic_,      path_topic_);
  private_nh.param("min_period",      min_period_,      min_period_);
  private_nh.param("downsample_step", downsample_step_, downsample_step_);
  private_nh.param("filter_outside",  filter_outside_,  filter_outside_);

  // ★新增参数
  private_nh.param("clip_from_nearest", clip_from_nearest_, clip_from_nearest_);  // ★新增
  private_nh.param("tf_lookup_timeout", tf_lookup_timeout_, tf_lookup_timeout_);  // ★新增
  private_nh.param("tf_use_zero_time",  tf_use_zero_time_,  tf_use_zero_time_);   // ★新增

  // 订阅自定义路径（降延迟）
  ros::TransportHints hints; hints.tcpNoDelay();
  path_sub_ = private_nh.subscribe(path_topic_, 1, &CustomPathPlanner::pathCb, this, hints);

  ROS_INFO_STREAM("CustomPathPlanner initialized. global_frame=" << global_frame_
                  << " topic=" << path_topic_
                  << " min_period=" << min_period_
                  << " downsample=" << downsample_step_
                  << " filter_outside=" << (filter_outside_?"true":"false")
                  << " clip_from_nearest=" << (clip_from_nearest_?"true":"false")
                  << " tf_use_zero_time=" << (tf_use_zero_time_?"true":"false")
                  << " tf_lookup_timeout=" << tf_lookup_timeout_ << "s");

  initialized_ = true;
}

void CustomPathPlanner::pathCb(const nav_msgs::Path& msg)
{
  if (!initialized_) return;

  std::vector<geometry_msgs::PoseStamped> tmp;
  tmp.reserve(msg.poses.size());

  costmap_2d::Costmap2D* cm = costmap_ros_ ? costmap_ros_->getCostmap() : nullptr;
  unsigned int mx, my;
  const int step = std::max(1, downsample_step_);

  for (size_t i = 0; i < msg.poses.size(); i += (size_t)step) {
    geometry_msgs::PoseStamped p = msg.poses[i];

    // ★修改1：统一到 global_frame，TF 查“最新”或给定 stamp
    const ros::Time tf_time = tf_use_zero_time_ ? ros::Time(0) : p.header.stamp;  // ★关键
    if (p.header.frame_id.empty())
      p.header.frame_id = global_frame_; // 防御

    if (p.header.frame_id != global_frame_) {
      try {
        if (!tf_buffer_.canTransform(global_frame_, p.header.frame_id, tf_time, ros::Duration(tf_lookup_timeout_))) {
          ROS_WARN_THROTTLE(1.0, "CustomPathPlanner: no TF %s->%s at time %.3f",
                            p.header.frame_id.c_str(), global_frame_.c_str(), tf_time.toSec());
          continue;
        }
        geometry_msgs::TransformStamped tf =
            tf_buffer_.lookupTransform(global_frame_, p.header.frame_id, tf_time, ros::Duration(tf_lookup_timeout_));
        tf2::doTransform(p, p, tf);
      } catch (const std::exception& e) {
        ROS_WARN_THROTTLE(1.0, "CustomPathPlanner: transform %s->%s failed: %s",
                          p.header.frame_id.c_str(), global_frame_.c_str(), e.what());
        continue;
      }
    }

    // 越界过滤（避免 Off Map 刷屏）
    if (filter_outside_ && cm) {
      if (!cm->worldToMap(p.pose.position.x, p.pose.position.y, mx, my)) {
        continue;
      }
    }

    // ★修改2：统一 header 到 map，并将 stamp 设置为 0（让下游取“最新”变换）
    p.header.frame_id = global_frame_;
    p.header.stamp    = ros::Time(0);  // ★关键

    // 修正空四元数
    if (p.pose.orientation.x==0 && p.pose.orientation.y==0 &&
        p.pose.orientation.z==0 && p.pose.orientation.w==0) {
      p.pose.orientation.w = 1.0;
    }

    // 去重
    if (!tmp.empty()) {
      const auto& q = tmp.back().pose.position;
      const double dx = p.pose.position.x - q.x;
      const double dy = p.pose.position.y - q.y;
      if (dx*dx + dy*dy < 1e-6) continue;
    }
    tmp.push_back(p);
  }

  if (tmp.empty()) {
    ROS_WARN_THROTTLE(2.0, "CustomPathPlanner: received path but all points filtered.");
    return;
  }

  cached_plan_.swap(tmp);
  last_path_stamp_ = msg.header.stamp;
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
  if (cached_plan_.empty()) {
    ROS_WARN_THROTTLE(2.0, "CustomPathPlanner: cached path is empty");
    return false;
  }

  // ★修改3：限频 + 从最近点裁剪
  const ros::Time now = ros::Time::now();
  if ((now - last_served_).toSec() < min_period_) {
    plan = cached_plan_;
    return true;
  }

  size_t idx0 = 0;
  if (clip_from_nearest_) {
    double best = std::numeric_limits<double>::infinity();
    for (size_t i = 0; i < cached_plan_.size(); ++i) {
      const auto& q = cached_plan_[i].pose.position;
      const double dx = q.x - start.pose.position.x;
      const double dy = q.y - start.pose.position.y;
      const double d2 = dx*dx + dy*dy;
      if (d2 < best) { best = d2; idx0 = i; }
    }
  }
  plan.assign(cached_plan_.begin() + idx0, cached_plan_.end());

  last_served_ = now;
  return true;
}

}  // namespace custom_global_planner

// 导出插件
PLUGINLIB_EXPORT_CLASS(custom_global_planner::CustomPathPlanner, nav_core::BaseGlobalPlanner)

