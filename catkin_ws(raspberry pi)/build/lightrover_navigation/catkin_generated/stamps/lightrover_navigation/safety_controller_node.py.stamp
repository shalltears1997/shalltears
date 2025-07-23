#!/usr/bin/env python3
import rospy
from std_msgs.msg import Bool
from geometry_msgs.msg import Twist
from topic_tools.srv import MuxSelect  # <-- 确保这个已经可以导入

class SafetyController:
    def __init__(self):
        rospy.init_node('safety_controller')

        self.edge_detected = False
        self.navigation_paused = False

        # 订阅边缘检测结果 (Bool)
        rospy.Subscriber('/edge_detected', Bool, self.edge_callback)

        # 创建发布者：安全速度指令 (给 mux 使用)
        self.safety_cmd_pub = rospy.Publisher('/safety_cmd_vel', Twist, queue_size=1)

        # ============ 这里是关键：等待 /mux/select 服务 & 创建 ServiceProxy ============
        rospy.wait_for_service('/mux/select')
        self.mux_select = rospy.ServiceProxy('/mux/select', MuxSelect)

        rospy.loginfo("SafetyController initialized. /mux/select is ready.")

    def edge_callback(self, msg):
        if msg.data and not self.navigation_paused:
            rospy.loginfo("Edge detected! Stopping navigation and retreating.")
            self.select_control('safety_cmd_vel')
            self.retreat()
            self.navigation_paused = True
        elif not msg.data and self.navigation_paused:
            rospy.loginfo("Edge no longer detected. Resuming navigation.")
            self.select_control('nav_cmd_vel')
            self.navigation_paused = False

    def select_control(self, topic_name):
        try:
            self.mux_select(topic_name)
            rospy.loginfo(f"Switched control to {topic_name}")
        except rospy.ServiceException as e:
            rospy.logerr(f"Failed to switch control: {e}")

    def retreat(self):
        # 发布后退指令
        twist = Twist()
        twist.linear.x = 0.08  # 后退速度
        retreat_duration = 1  # 后退持续时间（秒）

        rate = rospy.Rate(10)
        start_time = rospy.Time.now()
        while (rospy.Time.now() - start_time).to_sec() < retreat_duration:
            self.safety_cmd_pub.publish(twist)
            rate.sleep()

        # 停止机器人
        self.stop_robot()

    def stop_robot(self):
        twist = Twist()
        self.safety_cmd_pub.publish(twist)
        rospy.sleep(0.1)

    def run(self):
        rospy.spin()

if __name__ == '__main__':
    controller = SafetyController()
    controller.run()

