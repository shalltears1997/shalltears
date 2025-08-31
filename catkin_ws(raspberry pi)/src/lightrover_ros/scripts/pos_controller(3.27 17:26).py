#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# このプログラムは、ライトローバーを位置制御するためのノードです。

import rospy
import sys
from lightrover_ros.srv import *
import time
import math
import vs_wrc201_motor
from nav_msgs.msg import Odometry
from geometry_msgs.msg import Twist

write_msg = rospy.ServiceProxy('wrc201_i2c', Wrc201Msg)

# メモリマップアドレス
MU8_O_EN      = 0x10
MU8_TRIG      = 0x11
MS16_FB_PG0   = 0x20
MS16_FB_PG1   = 0x22

MS32_A_POS0   = 0x48
MS32_A_POS1   = 0x4c

MS16_T_OUT0   = 0x50
MS16_T_OUT1   = 0x52

MU16_FB_PCH0  = 0x30
MU16_FB_PCH1  = 0x32

liner_x       = 0.0
angular_z     = 0.0

current_v     = [0.0, 0.0]
target_rover_v= [0.0, 0.0]

# 車輪間距離の半分
ROVER_D = 0.143 / 2.0

motor_controller = vs_wrc201_motor.VsWrc201Motor()

# --- CHANGE START ---
# 新增：记录最后一次收到 /rover_drive 指令的时间，并设置超时阈值
last_cmd_time = None
CMD_TIMEOUT = 1.0  # 超时阈值，秒
# --- CHANGE END ---

def cb_get_rover_v(data):
    global liner_x, angular_z, current_v, ROVER_D, target_rover_v
    liner_x = data.twist.twist.linear.x
    angular_z = data.twist.twist.angular.z

    # 現在の直進・旋回速度から左右モータの現在の回転速度を算出
    current_v[1] = (liner_x + ROVER_D * angular_z)
    current_v[0] = -1.0 * (liner_x - ROVER_D * angular_z)

    # 現在のモータ回転速度と目標のモータ回転速度をPOSコントローラに入力
    # 左右モータへの出力値を算出
    output = motor_controller.pos_controll(current_v, target_rover_v)
    drive_motor(output[0], output[1])

def cb_set_target_v(data):
    global ROVER_D, target_rover_v
    # --- CHANGE START ---
    # 每次收到新的速度指令，就刷新 last_cmd_time
    global last_cmd_time
    last_cmd_time = rospy.Time.now()
    # --- CHANGE END ---

    # 目標直進・旋回速度から左右モータの目標回転速度を算出
    target_rover_v[1] = (data.linear.x + ROVER_D * data.angular.z)
    target_rover_v[0] = -1.0 * (data.linear.x - ROVER_D * data.angular.z)

# --- CHANGE START ---
def drive_motor(r_speed, l_speed):
    """
    原先的drive_motor, 但在此把浮点转成int, 避免SerializationError
    """
    r_speed_int = int(round(r_speed))
    l_speed_int = int(round(l_speed))

    write_msg(MS32_A_POS0, r_speed_int, 4, 'w')
    write_msg(MS32_A_POS1, l_speed_int, 4, 'w')
    write_msg(MU8_TRIG, 0x03, 1, 'w')
# --- CHANGE END ---

# --- CHANGE START ---
def check_timeout(event):
    """
    定时器回调：周期检查若超过CMD_TIMEOUT秒没收到rover_drive指令，则发0给电机停止
    """
    global last_cmd_time, CMD_TIMEOUT
    if last_cmd_time is None:
        return

    elapsed = (rospy.Time.now() - last_cmd_time).to_sec()
    if elapsed > CMD_TIMEOUT:
        # 超时 => 停车
        drive_motor(0.0, 0.0)
# --- CHANGE END ---

def pos_cntrl():
    rospy.init_node('pos_controller', anonymous=True)
    rospy.wait_for_service('wrc201_i2c')

    rospy.loginfo('Start POS Controll')

    write_msg(MU8_O_EN, 0x00, 1, 'w')           # モータ出力禁止
    write_msg(MU8_TRIG, 0x0c, 1, 'w')           # エンコーダリセット
    write_msg(MS16_FB_PG0, 0x0080, 2, 'w')      # モータ0位置補償Pゲイン設定
    write_msg(MS16_FB_PG1, 0x0080, 2, 'w')      # モータ1位置補償Pゲイン設定
    write_msg(MU16_FB_PCH0, 0x09C4, 2, 'w')     # モータ0最低出力値設定
    write_msg(MU16_FB_PCH1, 0x09C4, 2, 'w')     # モータ1最低出力値設定
    write_msg(MU8_O_EN, 0x03, 1, 'w')           # モータ出力許可

    # 订阅里程计, 并根据速度=odom.twist.twist传来的数据来做pos控制
    rospy.Subscriber('odom', Odometry, cb_get_rover_v)
    # 订阅上层速度指令(rover_drive)
    rospy.Subscriber('rover_drive', Twist, cb_set_target_v)

    # --- CHANGE START ---
    global last_cmd_time
    last_cmd_time = rospy.Time.now()

    # 每0.1秒检查一次是否超时
    rospy.Timer(rospy.Duration(0.1), check_timeout)
    # --- CHANGE END ---

    rospy.spin()

if __name__ == '__main__':
    pos_cntrl()

