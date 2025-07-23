#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
face_recognizer_no_yunet.py
示例：使用OpenCV HaarCascade进行人脸检测 + LBPH简单人脸识别
并从Realsense深度图获取人脸中心距离Z及坐标(相机坐标系).

订阅:
  1) /camera/color/image_raw_uncompressed (来自 image_transport republish)
  2) /camera/depth/image_rect_raw         (raw 16UC1 或 32FC1)

需要:
  - 一个haarcascade xml: haarcascade_frontalface_default.xml (随OpenCV安装)
  - 预先训练好的LBPH模型 (model.xml) (示例)
"""

import os
import sys
import cv2
import rospy
import message_filters
import numpy as np
from sensor_msgs.msg import Image, CameraInfo
from cv_bridge import CvBridge

class FaceRecognizerNoYuNet:
    def __init__(self):
        rospy.loginfo("Initializing FaceRecognizerNoYuNet node...")

        self.bridge = CvBridge()

        # 1) 订阅彩色 + 深度
        color_sub = message_filters.Subscriber("/camera/color/image_raw_uncompressed", Image)
        depth_sub = message_filters.Subscriber("/camera/depth/image_rect_raw", Image)
        sync = message_filters.ApproximateTimeSynchronizer([color_sub, depth_sub], queue_size=10, slop=0.1)
        sync.registerCallback(self.sync_callback)

        # 2) 人脸检测器 (Haar Cascade)
        # 路径: 你可自己指定, 下面是OpenCV默认
        self.face_cascade_path = "/usr/share/opencv4/haarcascades/haarcascade_frontalface_default.xml"
        if not os.path.exists(self.face_cascade_path):
            rospy.logwarn(f"Haarcascade not found at {self.face_cascade_path}")
        self.face_cascade = cv2.CascadeClassifier(self.face_cascade_path)

        # 3) 人脸识别器 (LBPH)
        # 假设你有个 model.xml 训练好的
        self.recognizer = cv2.face.LBPHFaceRecognizer_create()
        model_path = "/home/niitsuma/catkin_ws/src/my_realsense_demo/model.xml"  # 假设
        if os.path.exists(model_path):
            self.recognizer.read(model_path)
            rospy.loginfo("LBPH model loaded.")
        else:
            rospy.logwarn(f"No LBPH model found at {model_path}. We'll fallback to unknown.")

        # 4) 如果想计算3D坐标，需要相机内参
        #   典型: fx, fy, cx, cy
        #   可以订阅 /camera/color/camera_info or /camera/depth/camera_info
        #   这里简单用一些示例数
        self.fx = 615.0
        self.fy = 615.0
        self.cx = 320.0
        self.cy = 240.0
        # 具体数值请根据 realsense /camera_info 获取

        rospy.loginfo("FaceRecognizerNoYuNet setup done.")

    def sync_callback(self, color_msg, depth_msg):
        # 1) 转换图像
        color_img = self.bridge.imgmsg_to_cv2(color_msg, "bgr8")
        depth_img = self.bridge.imgmsg_to_cv2(depth_msg, "passthrough")  # 16UC1

        if color_img is None or depth_img is None:
            return

        # 2) 人脸检测 (Haar)
        gray = cv2.cvtColor(color_img, cv2.COLOR_BGR2GRAY)
        faces = self.face_cascade.detectMultiScale(
            gray, scaleFactor=1.1, minNeighbors=5, minSize=(40,40))

        for (x,y,w,h) in faces:
            # 3) 人脸识别
            roi_gray = gray[y:y+h, x:x+w]
            # LBPH需要 100x100?
            roi_resized = cv2.resize(roi_gray, (100,100))
            label, confidence = self.recognizer.predict(roi_resized) if hasattr(self.recognizer,"predict") else ( -1, 999 )
            name = f"ID:{label}" if label>=0 else "unknown"

            # 4) 获取人脸中心的深度
            cx_face = x + w//2
            cy_face = y + h//2
            if 0 <= cy_face < depth_img.shape[0] and 0 <= cx_face < depth_img.shape[1]:
                depth_value = depth_img[cy_face, cx_face]
            else:
                depth_value = 0

            # 5) 将 (cx_face, cy_face, depth_value) => 3D坐标 (X, Y, Z) (相机坐标系)
            # depth_value单位 mm? or meter? 需查看 realsense config
            Z = depth_value * 0.001  # if mm => meter
            # X = (u - cx)/fx * Z
            # Y = (v - cy)/fy * Z
            X = (cx_face - self.cx)/self.fx * Z
            Y = (cy_face - self.cy)/self.fy * Z

            # 6) 在图像上显示
            cv2.rectangle(color_img, (x,y), (x+w, y+h), (0,255,0), 2)
            info_text = f"{name} conf={confidence:.1f}"
            cv2.putText(color_img, info_text, (x, y-10), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0,255,0),2)

            dist_text = f"XYZ=({X:.2f},{Y:.2f},{Z:.2f}m)"
            cv2.putText(color_img, dist_text, (x,y+h+20), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0,255,0),2)

        cv2.imshow("FaceDetection with Depth( raw )", color_img)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            rospy.signal_shutdown("User pressed q in OpenCV window.")

def main():
    rospy.init_node("face_recognizer_no_yunet")

    FaceRecognizerNoYuNet()
    rospy.spin()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()
