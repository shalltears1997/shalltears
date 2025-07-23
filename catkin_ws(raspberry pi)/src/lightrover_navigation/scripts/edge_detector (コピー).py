#!/usr/bin/env python3
import rospy
from std_msgs.msg import Int16, Bool

class EdgeDetector:
    def __init__(self):
        rospy.init_node('edge_detector')
        self.threshold = rospy.get_param('~distance_threshold', 20)  # 设置距离增加量阈值，单位毫米
        self.edge_pub = rospy.Publisher('/edge_detected', Bool, queue_size=10)
        self.previous_distances = {}

        # 传感器话题列表
        self.sensor_topics = ['/sensor1/distance', '/sensor2/distance', '/sensor3/distance']
        for topic in self.sensor_topics:
            rospy.Subscriber(topic, Int16, self.sensor_callback, callback_args=topic)

    def sensor_callback(self, msg, topic):
        sensor_id = topic  # 使用话题名称作为传感器 ID
        current_distance = msg.data

        # 检查是否有之前的距离值
        if sensor_id in self.previous_distances:
            previous_distance = self.previous_distances[sensor_id]
            distance_increase = current_distance - previous_distance

            # 检测距离增加量是否超过阈值
            if distance_increase > self.threshold:
                rospy.loginfo(f"Edge detected by {sensor_id}: distance increased by {distance_increase} mm")
                # 发布检测到边缘的标志
                self.edge_pub.publish(True)
                return
        # 如果未检测到边缘，发布 False
        self.edge_pub.publish(False)

        # 更新之前的距离值
        self.previous_distances[sensor_id] = current_distance

    def spin(self):
        rospy.spin()

if __name__ == '__main__':
    try:
        detector = EdgeDetector()
        detector.spin()
    except rospy.ROSInterruptException:
        pass

