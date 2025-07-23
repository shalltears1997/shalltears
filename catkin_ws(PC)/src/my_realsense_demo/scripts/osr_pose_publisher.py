#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import rospy
import math
import time
import message_filters
import tf
import tf.transformations as tft

from geometry_msgs.msg import PoseArray, PoseWithCovarianceStamped
from std_msgs.msg import String, Float32MultiArray

class OsrPosePublisherNoSync:
    def __init__(self):
        rospy.init_node("osr_pose_publisher_no_sync", anonymous=True)

        # 发布 /osr_pose => [owner_x,y,z, stranger_x,y,z, robot_x,y,yaw]
        self.pub_osr = rospy.Publisher("/osr_pose", Float32MultiArray, queue_size=1)

        # 分别订阅
        self.sub_posearr = rospy.Subscriber("/face_global_positions", PoseArray, self.posearr_callback, queue_size=1)
        self.sub_id      = rospy.Subscriber("/face_global_positions_id",     String,    self.id_callback,   queue_size=1)

        # 订阅 /amcl_pose => 机器人
        self.sub_robot   = rospy.Subscriber("/amcl_pose", PoseWithCovarianceStamped, 
                                            self.robot_callback, queue_size=1)

        # 最新数据
        self.latest_posearray = None        # PoseArray
        self.latest_id_str    = ""          # "1,2"
        self.robot_x          = 0.0
        self.robot_y          = 0.0
        self.robot_yaw        = 0.0

        # 定时器: 每0.1秒 => 合并 + 发布 /osr_pose
        rospy.Timer(rospy.Duration(0.1), self.publish_timer_cb)

        rospy.loginfo("[osr_pose_publisher_no_sync] init done => separate sub + timer => /osr_pose")

    def posearr_callback(self, msg):
        """只保存最新的 PoseArray。"""
        self.latest_posearray = msg

    def id_callback(self, msg):
        """只保存最新的 ID string。"""
        self.latest_id_str = msg.data

    def robot_callback(self, msg):
        """解析机器人 x,y,yaw"""
        x = msg.pose.pose.position.x
        y = msg.pose.pose.position.y
        q = msg.pose.pose.orientation
        (r,p,yaw) = tft.euler_from_quaternion([q.x,q.y,q.z,q.w])
        self.robot_x  = x
        self.robot_y  = y
        self.robot_yaw= math.degrees(yaw)

    def publish_timer_cb(self, event):
        """
        每0.1秒主动合并: 
         1) latest_posearray
         2) latest_id_str
         3) robot_x,y,yaw
        => 解析 ID=1 => owner; ID=2 => stranger => /osr_pose
        """
        if self.latest_posearray is None:
            # 还没收到/face_global_positions => 不发
            return

        # 先默认 (owner, stranger) = (0,0,0)
        owner_x,owner_y,owner_z= 0.0,0.0,0.0
        stranger_x,stranger_y,stranger_z= 0.0,0.0,0.0

        # 尝试解析 ID=1 => owner, ID=2 => stranger
        id_list = []
        if self.latest_id_str:
            id_list = self.latest_id_str.split(",")

        faces = self.latest_posearray.poses
        cnt = min(len(id_list), len(faces))

        for i in range(cnt):
            try:
                pid = int(id_list[i])
            except:
                pid= -1
            p= faces[i]
            x= p.position.x
            y= p.position.y
            z= p.position.z

            if pid==1:
                owner_x,owner_y,owner_z= x,y,z
            elif pid==2:
                stranger_x,stranger_y,stranger_z= x,y,z
        
        # 机器人的
        rx, ry, ryaw= self.robot_x, self.robot_y, self.robot_yaw

        # 组装
        arr= Float32MultiArray()
        arr.data= [
            owner_x,    owner_y,    owner_z,
            stranger_x, stranger_y, stranger_z,
            rx,         ry,         ryaw
        ]
        self.pub_osr.publish(arr)

        rospy.loginfo_throttle(5.0,
            f"[no_sync] /osr_pose => O=({owner_x:.1f},{owner_y:.1f}),S=({stranger_x:.1f},{stranger_y:.1f}),R=({rx:.1f},{ry:.1f},{ryaw:.1f})"
        )

def main():
    node= OsrPosePublisherNoSync()
    rospy.spin()

if __name__=="__main__":
    main()

