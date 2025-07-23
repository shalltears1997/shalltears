#!/usr/bin/env python3
import rospy
from std_msgs.msg import Int16, Bool, String

class EdgeDetector:
    def __init__(self):
        rospy.init_node('edge_detector')

        # 距离阈值，单位 mm
        self.threshold = rospy.get_param('~distance_threshold', 25)

        # ----(A) 发布 Bool 话题：/edge_detected（给 safety_controller 用）----
        self.edge_pub_bool = rospy.Publisher('/edge_detected', Bool, queue_size=10)
        
        # ----(B) 发布 String 话题：/edge_detected_sensor（给 obstacle_dual_publisher 用）----
        self.edge_pub_sensor = rospy.Publisher('/edge_detected_sensor', String, queue_size=10)

        # 存储上一帧距离值
        self.previous_distances = {}

        # 三个传感器话题
        self.sensor_topics = ['/sensor1/distance', '/sensor2/distance', '/sensor3/distance']
        
        # 分别订阅三个距离
        for topic in self.sensor_topics:
            rospy.Subscriber(topic, Int16, self.sensor_callback, callback_args=topic)

    def sensor_callback(self, msg, topic):
        """对每个传感器距离进行阈值检测，如果距离突然增大，视为检测到桌面边缘。"""
        sensor_id = topic
        current_distance = msg.data

        if sensor_id in self.previous_distances:
            previous_distance = self.previous_distances[sensor_id]
            distance_increase = current_distance - previous_distance

            # 如果距离突然增大超过 self.threshold => 认定为桌面边缘
            if distance_increase > self.threshold:
                rospy.loginfo(f"[EdgeDetector] Edge detected by {sensor_id}, increased {distance_increase} mm")
                
                # ----(1) 发布 Bool=True 给 /edge_detected----
                self.edge_pub_bool.publish(True)
                
                # ----(2) 发布 String=sensor_id 给 /edge_detected_sensor----
                self.edge_pub_sensor.publish(sensor_id)
                
                return  # 提前return，不执行后面的False发布

        # 如果没有检测到边缘，就发布 Bool=False
        self.edge_pub_bool.publish(False)
        # 更新previous_distance
        self.previous_distances[sensor_id] = current_distance

    def spin(self):
        rospy.spin()

if __name__ == '__main__':
    try:
        node = EdgeDetector()
        node.spin()
    except rospy.ROSInterruptException:
        pass

