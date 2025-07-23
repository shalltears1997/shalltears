#!/usr/bin/env python3
import rospy
from sensor_msgs.msg import Range
import board
import busio
import adafruit_vl53l0x

class VL53L0XNode:
    def __init__(self):
        rospy.init_node('vl53l0x_node')
        self.pub = rospy.Publisher('/vl53l0x/range', Range, queue_size=1)
        self.rate = rospy.Rate(10)  # 10Hz
        self.frame_id = 'vl53l0x_link'  # 与URDF中的frame_id一致

        # 初始化I2C总线和传感器
        i2c = busio.I2C(board.SCL, board.SDA)
        self.sensor = adafruit_vl53l0x.VL53L0X(i2c)

        self.min_range = 0.01  # 最小测量距离（米）
        self.max_range = 2.0   # 最大测量距离（米）
        self.field_of_view = 0.436  # 约25度，视情况调整

    def run(self):
        while not rospy.is_shutdown():
            distance = self.sensor.range / 1000.0  # 将毫米转换为米
            if distance == 0 or distance > self.max_range:
                distance = float('inf')  # 无效读数

            msg = Range()
            msg.header.stamp = rospy.Time.now()
            msg.header.frame_id = self.frame_id
            msg.radiation_type = Range.INFRARED
            msg.field_of_view = self.field_of_view
            msg.min_range = self.min_range
            msg.max_range = self.max_range
            msg.range = distance
            self.pub.publish(msg)
            self.rate.sleep()

if __name__ == '__main__':
    try:
        node = VL53L0XNode()
        node.run()
    except rospy.ROSInterruptException:
        pass

