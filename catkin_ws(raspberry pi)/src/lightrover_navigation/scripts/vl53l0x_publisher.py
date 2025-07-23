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
    rate = rospy.Rate(5)  # 5Hz

    # 初始化 I2C 总线 and TCA9548A
    i2c = busio.I2C(board.SCL, board.SDA)
    tca = adafruit_tca9548a.TCA9548A(i2c)

    # 初始化 VL53L0X 传感器
    sensors = []
    sensor_channels = [0, 1, 2]  # channel0,1,2 for three sensors
    for ch in sensor_channels:
        sensor_i2c = tca[ch]  # 访问 channel ch
        sensor = adafruit_vl53l0x.VL53L0X(sensor_i2c)
        sensors.append(sensor)
        rospy.loginfo(f"Initialized VL53L0X on TCA channel {ch}")

    while not rospy.is_shutdown():
        # 逐个读取
        if len(sensors) >= 3:
            dist1 = sensors[0].range
            dist2 = sensors[1].range
            dist3 = sensors[2].range

            pub1.publish(dist1)
            pub2.publish(dist2)
            pub3.publish(dist3)
        else:
            rospy.logwarn("Not enough VL53 sensors, skipping publish...")

        rate.sleep()

if __name__ == '__main__':
    try:
        sensor_publisher()
    except rospy.ROSInterruptException:
        pass

