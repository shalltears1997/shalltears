#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
grove_imu_node.py
在 Raspberry Pi 上通过 TCA9548A(0x70) 选通 channel3 (0x08) 后，
访问 LSM6DS3 IMU (I2C地址=0x6A)，初始化后读取加速度/角速度并发布 /imu/data_raw。

特点:
 - 仅做 2D用途 => az=0.0, gx=0.0, gy=0.0, 只保留 gz
 - 如果想要3D，可把 "az=0.0" 去掉, 并保留 gx,gy

LSM6DS3 关键寄存器:
 - WHO_AM_I(0x0F) => 0x69
 - CTRL1_XL(0x10) => 配置加速度 ODR/FS
 - CTRL2_G(0x11)  => 配置陀螺 ODR/FS
 - CTRL3_C(0x12)  => BDU/IF_INC等
 - OUTX_L_G ~ OUTZ_H_G (0x22~0x27) => 陀螺
 - OUTX_L_XL ~ OUTZ_H_XL (0x28~0x2D) => 加速度

地址: 0x6A (或 0x6B)
"""

import rospy
import smbus
import math
import time
import os
from sensor_msgs.msg import Imu

# TCA9548A
MUX_ADDR = 0x70
MUX_REG  = 0x00
MUX_CH3  = 0x08  # 1<<3 => channel3

# LSM6DS3 registers
REG_WHO_AM_I  = 0x0F
REG_CTRL1_XL  = 0x10
REG_CTRL2_G   = 0x11
REG_CTRL3_C   = 0x12

REG_OUTX_L_G  = 0x22  # gx L
REG_OUTX_H_G  = 0x23  # gx H
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

# LSM6DS3 scale factors (默认 ±2g, ±245 dps)
# => 加速度: 0.061 mg/LSB => 0.000061 g => *9.81 => ~0.0006 m/s^2
ACC_LSB_2G   = 0.000061 * 9.81  # ~ 0.00059821 m/s^2
# => 陀螺: 8.75 mdps/LSB => 0.00875 deg/s => 0.00015272 rad/s
GYR_LSB_245DPS = 0.00015272

class GroveIMUNode:
    def __init__(self):
        rospy.init_node('grove_imu_node', anonymous=True)

        self.pub_imu = rospy.Publisher('/imu/data_raw', Imu, queue_size=10)

        # 1) 选通 TCA9548A channel3
        self.select_mux_channel()

        # 2) 打开 i2c bus, 设地址=0x6A
        self.bus = smbus.SMBus(1)
        self.imu_addr = 0x6A

        # 3) 初始化 LSM6DS3
        self.init_lsm6ds3()

        # 发布频率 (默认 50)
        rate_hz = rospy.get_param('~rate', 50)
        self.rate = rospy.Rate(rate_hz)

        rospy.loginfo("LSM6DS3 IMU Node started, /imu/data_raw @ %dHz", rate_hz)
        while not rospy.is_shutdown():
            self.publish_imu()
            self.rate.sleep()

    def select_mux_channel(self):
        """
        写 i2cset -y 1 0x70 0x00 0x08 => 选通 TCA9548A channel3
        """
        cmd = f" i2cset -y 1 {MUX_ADDR} {MUX_REG} {MUX_CH3}"
        ret = os.system(cmd)
        if ret != 0:
            rospy.logwarn("i2cset for mux channel3 failed. Possibly no sudo or user canceled.")
        else:
            rospy.loginfo("Selected TCA9548A channel3 successfully.")

    def write_reg(self, reg, val):
        self.bus.write_byte_data(self.imu_addr, reg, val)
        time.sleep(0.01)

    def init_lsm6ds3(self):
        """
        初始化 LSM6DS3:
         - CTRL3_C => BDU=1, IF_INC=1
         - CTRL1_XL => ODR=104Hz, ±2g
         - CTRL2_G => ODR=104Hz, ±245 dps
        """
        # 先读 WHO_AM_I(0x0F),应=0x69
        who_am_i = self.bus.read_byte_data(self.imu_addr, REG_WHO_AM_I)
        rospy.loginfo("LSM6DS3 WHO_AM_I=0x%02X (expect 0x69)", who_am_i)

        # CTRL3_C(0x12):
        #  BDU=1( bit6 ), IF_INC=1( bit2 ), => 0x44 => 0100_0100
        self.write_reg(REG_CTRL3_C, 0x44)

        # CTRL1_XL(0x10):
        #  ODR=104Hz(0x4<<4=0x40), ±2g(00), => 0x40 => 0100_0000 => 0x40
        self.write_reg(REG_CTRL1_XL, 0x40)

        # CTRL2_G(0x11):
        #  ODR=104Hz(0x4<<4=0x40), ±245 dps(00) => 0x40
        self.write_reg(REG_CTRL2_G, 0x40)

        rospy.loginfo("LSM6DS3(0x6A) init done. ODR=104Hz,±2g,±245dps")

    def read_word_2c(self, low_reg, high_reg):
        """
        读取16位有符号数: low, high
        """
        low  = self.bus.read_byte_data(self.imu_addr, low_reg)
        high = self.bus.read_byte_data(self.imu_addr, high_reg)
        val  = (high << 8) | low
        if val >= 0x8000:
            val -= 65536
        return val

    def read_imu_data(self):
        # 读取陀螺
        gx_l = self.read_word_2c(REG_OUTX_L_G, REG_OUTX_H_G)  # actually 2 regs: 0x22,0x23
        gy_l = self.read_word_2c(REG_OUTY_L_G, REG_OUTY_H_G)
        gz_l = self.read_word_2c(REG_OUTZ_L_G, REG_OUTZ_H_G)

        # 读取加速度
        ax_l = self.read_word_2c(REG_OUTX_L_XL, REG_OUTX_H_XL)  # 0x28,0x29
        ay_l = self.read_word_2c(REG_OUTY_L_XL, REG_OUTY_H_XL)
        az_l = self.read_word_2c(REG_OUTZ_L_XL, REG_OUTZ_H_XL)

        # 转成物理量
        #  加速度 => raw * ACC_LSB_2G => m/s^2
        #  陀螺 => raw * GYR_LSB_245DPS => rad/s
        ax = ax_l * ACC_LSB_2G
        ay = ay_l * ACC_LSB_2G
        az = az_l * ACC_LSB_2G

        gx = gx_l * GYR_LSB_245DPS
        gy = gy_l * GYR_LSB_245DPS
        gz = gz_l * GYR_LSB_245DPS

        # 2D用途 => az=0, gx=0, gy=0
        az = 0.0
        gx = 0.0
        gy = 0.0

        return ax, ay, az, gx, gy, gz

    def publish_imu(self):
        ax, ay, az, gx, gy, gz = self.read_imu_data()

        # 构造 Imu消息
        from sensor_msgs.msg import Imu
        imu_msg = Imu()
        imu_msg.header.stamp = rospy.Time.now()
        imu_msg.header.frame_id = "imu_link"

        # orientation未知 => set orientation_covariance[0] = -1
        imu_msg.orientation_covariance[0] = -1

        # 角速度
        imu_msg.angular_velocity.x = gx
        imu_msg.angular_velocity.y = gy
        imu_msg.angular_velocity.z = gz

        # 线加速度
        imu_msg.linear_acceleration.x = ax
        imu_msg.linear_acceleration.y = ay
        imu_msg.linear_acceleration.z = az

        self.pub_imu.publish(imu_msg)

def main():
    rospy.loginfo("Starting LSM6DS3 grove_imu_node with channel3 mux selection.")
    GroveIMUNode()

if __name__ == '__main__':
    try:
        main()
    except rospy.ROSInterruptException:
        pass

