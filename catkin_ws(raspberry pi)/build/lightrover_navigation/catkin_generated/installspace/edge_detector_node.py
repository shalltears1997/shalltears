#!/usr/bin/env python3
import rospy
from sensor_msgs.msg import Range
from std_msgs.msg import Bool

class EdgeDetector:
    def __init__(self):
        rospy.init_node('edge_detector')
        self.distance_threshold = rospy.get_param('~distance_threshold', 0.1)  # 根据需要调整
        self.edge_detected_pub = rospy.Publisher('/edge_detected', Bool, queue_size=1)
        rospy.Subscriber('/vl53l0x/range', Range, self.distance_callback)
        self.edge_detected = False

    def distance_callback(self, msg):
        if msg.range >= self.distance_threshold and msg.range <= msg.max_range:
            if not self.edge_detected:
                self.edge_detected = True
                rospy.loginfo("Edge detected!")
                self.edge_detected_pub.publish(True)
        else:
            if self.edge_detected:
                self.edge_detected = False
                rospy.loginfo("Edge no longer detected.")
                self.edge_detected_pub.publish(False)

    def run(self):
        rospy.spin()

if __name__ == '__main__':
    detector = EdgeDetector()
    detector.run()
