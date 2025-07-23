#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import rospy
import cv2
import numpy as np
from sensor_msgs.msg import Image
from cv_bridge import CvBridge

class FaceRecognizerROS:
    def __init__(self):
        rospy.loginfo("Initializing FaceRecognizerROS node...")
        self.bridge = CvBridge()

        # Subscribe uncompressed color
        self.sub_color = rospy.Subscriber(
            "/camera/color/image_raw_uncompressed",
            Image,
            self.color_cb, queue_size=1
        )
        # Subscribe uncompressed depth
        self.sub_depth = rospy.Subscriber(
            "/camera/depth/image_rect_raw_uncompressed",
            Image,
            self.depth_cb, queue_size=1
        )
        rospy.loginfo("Subscription set up. Waiting for images...")

    def color_cb(self, msg):
        color_img = self.bridge.imgmsg_to_cv2(msg, "bgr8")
        # Display
        cv2.imshow("Color", color_img)
        cv2.waitKey(1)

    def depth_cb(self, msg):
        # Possibly 16UC1 or 32FC1
        depth_img = self.bridge.imgmsg_to_cv2(msg, "passthrough")
        # Not displayed by default. You can do further processing here.

def main():
    rospy.init_node("face_recognizer_ros", anonymous=True)
    node = FaceRecognizerROS()
    rospy.spin()
    cv2.destroyAllWindows()

if __name__=="__main__":
    main()
