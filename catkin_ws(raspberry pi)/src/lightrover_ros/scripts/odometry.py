#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# このプログラムは、ライトローバーのオドメトリを取得するためのノードです。
# ★修改方针：仅更换时间基准为 ROS 时间；运动学与原脚本保持一致。

import rospy
import sys
from lightrover_ros.srv import *
import time
import math
from geometry_msgs.msg import PoseStamped, TwistStamped, Quaternion, TransformStamped
import tf
from nav_msgs.msg import Odometry

MS32_M_POS0 = 0x60
MS32_M_POS1 = 0x64

pre_count = [0.0, 0.0]
diff_count = [0, 0]

DIFF_COUNT_LIMIT = 1048575

# ★修改：不再用 time.time 做基准；改用 ROS 时间
pre_ros_time = None       # 上一帧 ROS 时间
diff_time = 0.0

WHEEL_CIRCUMFERENCE = 60.0*math.pi/1000
ENC_COUNTS_PER_TURN = 1188.024
ENC_PER_M = ENC_COUNTS_PER_TURN/WHEEL_CIRCUMFERENCE

ROVER_D = 0.143/2.0

x = 0.0
y = 0.0
th = 0.0

read_enc = rospy.ServiceProxy('wrc201_i2c', Wrc201Msg)

def getEncVal():
    try:
        # VS-WRC201上のマイコンからエンコーダ値を取得
        enc_a = read_enc(MS32_M_POS0, 0, 4, 'r')
        enc_b = read_enc(MS32_M_POS1, 0, 4, 'r')
        return enc_a.readData, enc_b.readData
    except rospy.ServiceException as e:
        rospy.loginfo('Service call failed: %s', e)

def calSpeed():
    """读取编码器，计算速度；用 ROS 时间来计算 diff_time。
       返回 (linear_x, angular_z, stamp_ros) —— stamp_ros 将用于 TF 与 Odom。"""
    global diff_time, pre_ros_time, diff_count, pre_count

    # ★修改：统一使用 ROS 时间
    now_ros = rospy.Time.now()
    if pre_ros_time is None:
        diff_time = 0.0
    else:
        diff_time = (now_ros - pre_ros_time).to_sec()
    pre_ros_time = now_ros
    if diff_time <= 0.0:
        diff_time = 1e-3  # 极小保护，避免除零

    enc_val = getEncVal()
    if enc_val is None:
        return None

    # 以前のエンコーダ値と現在のエンコーダ値の差を算出（原样保留）
    for i in range(2):
        if abs(enc_val[i] - pre_count[i]) < DIFF_COUNT_LIMIT:
            diff_count[i] = -1.0 * (enc_val[i] - pre_count[i])

    pre_count = enc_val

    # 各タイヤの移動距離を算出（原样保留）
    distance = [float(diff_count[0]) / ENC_PER_M, float(diff_count[1]) / ENC_PER_M]
    # 各タイヤの回転速度を算出（原样保留）
    speed = [distance[0] / diff_time, distance[1] / diff_time]

    # 本体の直進・旋回速度を算出（原样保留）
    linear_x = ((speed[0] - speed[1]) / 2.0)
    angular_z = -1.0 * ((speed[0] + speed[1]) / (2.0 * ROVER_D))

    # ★修改：把用于积分的时间戳也返回，供 TF/Odom 统一使用
    return linear_x, angular_z, now_ros

def cal_odometry(vx, vth):
    # （原样保留）用 diff_time 进行位姿积分
    global x, y, th
    delta_x = vx * math.cos(th) * diff_time
    delta_y = vx * math.sin(th) * diff_time
    delta_th = vth * diff_time
    x += delta_x
    y += delta_y
    th += delta_th

def lightrover_odometry():
    rospy.init_node('wrc201_odometry', anonymous=True)  # ★务必先 init_node 再用 ROS 时间
    rospy.wait_for_service('wrc201_i2c')

    odom_pub = rospy.Publisher('odom', Odometry, queue_size=50)
    odom_br  = tf.TransformBroadcaster()

    rate = rospy.Rate(30)

    # ★修改：先调用一次，建立 ROS 时间基线与初始计数
    first = calSpeed()

    while not rospy.is_shutdown():
        val = calSpeed()
        if val is None:
            rate.sleep()
            continue

        vx, vth, stamp_ros = val          # ★修改：拿到本次积分所用的 ROS 时间
        cal_odometry(vx, vth)

        # ★修改：TF 与 Odom 都用同一个 stamp_ros，确保链路一致
        odom_quat = tf.transformations.quaternion_from_euler(0, 0, th)

        odom_br.sendTransform((x, y, 0.0),
                              odom_quat,
                              stamp_ros,
                              "base_link",
                              "odom")

        odom = Odometry()
        odom.header.stamp = stamp_ros
        odom.header.frame_id = "odom"

        odom.pose.pose.position.x = x
        odom.pose.pose.position.y = y
        odom.pose.pose.position.z = 0.0
        odom.pose.pose.orientation.x = odom_quat[0]
        odom.pose.pose.orientation.y = odom_quat[1]
        odom.pose.pose.orientation.z = odom_quat[2]
        odom.pose.pose.orientation.w = odom_quat[3]

        odom.child_frame_id = "base_link"
        odom.twist.twist.linear.x  = vx
        odom.twist.twist.linear.y  = 0.0
        odom.twist.twist.angular.z = vth

        odom_pub.publish(odom)
        rate.sleep()

if __name__ == "__main__":
    rospy.loginfo('Start calculate odometry')
    try:
        lightrover_odometry()
    except rospy.ROSInterruptException:
        pass

