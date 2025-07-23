#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
grove_imu_node.py - 直接使用树莓派的 I2C /dev/i2c-1 访问 LSM6DS3(0x6A)
不再对TCA9548A做任何选通操作。
"""

import rospy
import smbus
import math
import time
from sensor_msgs.msg import Imu

# LSM6DS3 IMU的默认I2C地址(当SA0=0)= 0x6A (也可能是0x6B,看硬件)
IMU_ADDR = 0x6A

# 关键寄存器
REG_WHO_AM_I  = 0x0F
REG_CTRL1_XL  = 0x10
REG_CTRL2_G   = 0x11
REG_CTRL3_C   = 0x12
REG_OUTX_L_G  = 0x22
REG_OUTX_H_G  = 0x23
REG_OUTY_L_G  = 0x24
REG_OUTY_H_G  = 0x25
REG_OUTZ_L_G  = 0x26
REG_OUTZ_H_G  = 0x27
REG_OUTX_L_XL = 0x28
REG_OUTX_H_XL = 0x29
REG_OUTY_L_XL = 0x2A
REG_OUTY_H_XL = 0x2B
REG_OUTZ_L_XL = 0x2C
REG_OUTZ_H_XL = 0x2D

# 转换比例(默认±2g / ±245dps)
ACC_LSB_2G       = 0.000061 * 9.81  # ~0.000598 m/s^2
GYR_LSB_245DPS   = 0.00015272       # rad/s

class GroveIMUNode:
    def __init__(self):
        rospy.init_node('grove_imu_node', anonymous=True)

        # (1) 直接用主I2C总线
        self.bus = smbus.SMBus(1)

        # (2) 初始化 IMU
        self.init_lsm6ds3()

        # (3) 准备发布 /imu/data_raw
        self.pub_imu = rospy.Publisher('/imu/data_raw', Imu, queue_size=10)

        # (4) 设置发布频率(默认50Hz，或自行调节)
        rate_hz = rospy.get_param('~rate', 50)
        self.rate = rospy.Rate(rate_hz)

        rospy.loginfo("LSM6DS3 IMU Node started, /imu/data_raw @ %dHz", rate_hz)
        while not rospy.is_shutdown():
            self.publish_imu()
            self.rate.sleep()

    def write_reg(self, reg, val):
        self.bus.write_byte_data(IMU_ADDR, reg, val)
        time.sleep(0.001)

    def read_word_2c(self, low_reg, high_reg):
        low  = self.bus.read_byte_data(IMU_ADDR, low_reg)
        high = self.bus.read_byte_data(IMU_ADDR, high_reg)
        val  = (high << 8) | low
        if val >= 0x8000:
            val -= 65536
        return val

    def init_lsm6ds3(self):
        # 读取 WHO_AM_I(0x0F),应=0x69
        who_am_i = self.bus.read_byte_data(IMU_ADDR, REG_WHO_AM_I)
        rospy.loginfo("LSM6DS3 WHO_AM_I=0x%02X (expect 0x69)", who_am_i)

        # CTRL3_C(0x12): BDU=1(bit6), IF_INC=1(bit2) => 0x44
        self.write_reg(REG_CTRL3_C, 0x44)
        # CTRL1_XL(0x10): ODR=104Hz(0x4<<4=0x40), ±2g => 0x40
        self.write_reg(REG_CTRL1_XL, 0x40)
        # CTRL2_G(0x11): ODR=104Hz(0x4<<4=0x40), ±245dps => 0x40
        self.write_reg(REG_CTRL2_G, 0x40)

        rospy.loginfo("LSM6DS3(0x6A) init done. ODR=104Hz,±2g,±245dps")

    def read_imu_data(self):
        gx_l = self.read_word_2c(REG_OUTX_L_G,  REG_OUTX_H_G)
        gy_l = self.read_word_2c(REG_OUTY_L_G,  REG_OUTY_H_G)
        gz_l = self.read_word_2c(REG_OUTZ_L_G,  REG_OUTZ_H_G)
        ax_l = self.read_word_2c(REG_OUTX_L_XL, REG_OUTX_H_XL)
        ay_l = self.read_word_2c(REG_OUTY_L_XL, REG_OUTY_H_XL)
        az_l = self.read_word_2c(REG_OUTZ_L_XL, REG_OUTZ_H_XL)

        ax = ax_l * ACC_LSB_2G
        ay = ay_l * ACC_LSB_2G
        az = az_l * ACC_LSB_2G

        gx = gx_l * GYR_LSB_245DPS
        gy = gy_l * GYR_LSB_245DPS
        gz = gz_l * GYR_LSB_245DPS

        # 如果你只关心2D yaw，可以把az,gx,gy=0
        # az = 0.0
        # gx = 0.0
        # gy = 0.0

        return ax, ay, az, gx, gy, gz

    def publish_imu(self):
        ax, ay, az, gx, gy, gz = self.read_imu_data()

        imu_msg = Imu()
        imu_msg.header.stamp = rospy.Time.now()
        imu_msg.header.frame_id = "imu_link"  # 你的IMU坐标系

        # orientation未知 => set orientation_covariance[0] = -1
        imu_msg.orientation_covariance[0] = -1

        imu_msg.angular_velocity.x = gx
        imu_msg.angular_velocity.y = gy
        imu_msg.angular_velocity.z = gz

        imu_msg.linear_acceleration.x = ax
        imu_msg.linear_acceleration.y = ay
        imu_msg.linear_acceleration.z = az

        self.pub_imu.publish(imu_msg)

def main():
    rospy.loginfo("Starting LSM6DS3 grove_imu_node (direct on /dev/i2c-1)")
    GroveIMUNode()

if __name__ == '__main__':
    try:
        main()
    except rospy.ROSInterruptException:
        pass

