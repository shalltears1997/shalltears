#!/usr/bin/env python3
import rospy
from std_msgs.msg import String
from geometry_msgs.msg import Twist
from nav_msgs.msg import Odometry
import math

class EdgeAvoidanceController:
    def __init__(self):
        rospy.init_node('edge_avoidance_controller')
        self.cmd_pub = rospy.Publisher('edge_cmd_vel', Twist, queue_size=10)
        rospy.Subscriber('/edge_detected', String, self.edge_callback)
        rospy.Subscriber('/odom', Odometry, self.odom_callback)
        self.edge_detected = False
        self.moving_back = False
        self.starting_position = None
        self.current_position = None

        # 后退距离，单位：米
        self.backup_distance = 0.1  # 10 厘米

    def odom_callback(self, msg):
        self.current_position = msg.pose.pose.position

    def edge_callback(self, msg):
        if not self.moving_back:
            self.edge_detected = True
            self.starting_position = self.current_position
            rospy.loginfo("Edge detected. Moving backward.")
            self.move_backward()

    def move_backward(self):
        if self.starting_position is None or self.current_position is None:
            rospy.logwarn("Position data is not available.")
            return

        self.moving_back = True
        rate = rospy.Rate(10)
        distance_moved = 0.0

        while distance_moved < self.backup_distance and not rospy.is_shutdown():
            # 发布后退指令
            twist = Twist()
            twist.linear.x = -0.1  # 后退速度 0.1 m/s
            self.cmd_pub.publish(twist)

            # 计算已移动的距离
            dx = self.current_position.x - self.starting_position.x
            dy = self.current_position.y - self.starting_position.y
            distance_moved = math.sqrt(dx*dx + dy*dy)

            rate.sleep()

        # 停止机器人
        twist = Twist()
        self.cmd_pub.publish(twist)
        self.moving_back = False
        rospy.loginfo("Moved backward 10 cm. Resuming navigation.")

    def spin(self):
        rospy.spin()

if __name__ == '__main__':
    try:
        controller = EdgeAvoidanceController()
        controller.spin()
    except rospy.ROSInterruptException:
        pass

