#!/usr/bin/env python3
import rospy
from std_msgs.msg import Int16
import time
import board
import busio
import adafruit_tca9548a
import adafruit_vl53l0x

def sensor_publisher():
    rospy.init_node('vl53l0x_publisher')
    pub1 = rospy.Publisher('/sensor1/distance', Int16, queue_size=10)
    pub2 = rospy.Publisher('/sensor2/distance', Int16, queue_size=10)
    pub3 = rospy.Publisher('/sensor3/distance', Int16, queue_size=10)
    rate = rospy.Rate(10)  # 10Hz

    # 初始化 I2C 总线和多路复用器
    i2c = busio.I2C(board.SCL, board.SDA)
    tca = adafruit_tca9548a.TCA9548A(i2c)

    # 初始化传感器
    sensors = []
    sensor_channels = [0, 1, 2]  # 添加新的传感器通道 2
    for channel in sensor_channels:
        sensor_i2c = tca[channel]
        sensor = adafruit_vl53l0x.VL53L0X(sensor_i2c)
        sensors.append(sensor)
        rospy.loginfo(f"Initialized sensor on channel {channel}")

    while not rospy.is_shutdown():
        for i, sensor in enumerate(sensors):
            distance = sensor.range
            if i == 0:
                pub1.publish(distance)
            elif i == 1:
                pub2.publish(distance)
            elif i == 2:
                pub3.publish(distance)
        rate.sleep()

if __name__ == '__main__':
    try:
        sensor_publisher()
    except rospy.ROSInterruptException:
        pass

