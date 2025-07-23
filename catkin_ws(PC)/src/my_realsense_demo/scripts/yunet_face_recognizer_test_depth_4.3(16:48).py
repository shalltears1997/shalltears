#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import sys
import argparse
import glob
import numpy as np
import cv2
import rospy
import message_filters
from sensor_msgs.msg import Image
from cv_bridge import CvBridge

COSINE_THRESHOLD = 0.363

def match(recognizer, feature1, dictionary):
    for user_id, feature2 in dictionary:
        score = recognizer.match(feature1, feature2, cv2.FaceRecognizerSF_FR_COSINE)
        if score > COSINE_THRESHOLD:
            return True, (user_id, score)
    return False, ("unknown", 0.0)

class YuNetFaceNode:
    def __init__(self, person_name):
        rospy.loginfo("Initializing YuNetFaceNode with person=%s...", person_name)
        self.bridge = CvBridge()

        # 1) 加载特征
        dictionary_dir = f"/home/niitsuma/catkin_ws/src/yunet/{person_name}/{person_name}_faces_dictionary"
        self.dictionary = []
        files = glob.glob(os.path.join(dictionary_dir, "*.npy"))
        for file in files:
            feat = np.load(file)
            user_id = os.path.splitext(os.path.basename(file))[0]
            self.dictionary.append((user_id, feat))
        rospy.loginfo(f"Loaded {len(self.dictionary)} face features from {dictionary_dir}")

        # 2) 加载YuNet+SFace
        model_dir = "/home/niitsuma/catkin_ws/src/yunet"
        detector_onnx = os.path.join(model_dir, "face_detection_yunet_2023mar.onnx")
        recognizer_onnx = os.path.join(model_dir, "face_recognition_sface_2021dec.onnx")

        self.face_detector = cv2.FaceDetectorYN_create(detector_onnx, "", (0,0))
        self.face_recognizer = cv2.FaceRecognizerSF_create(recognizer_onnx, "")

        # 3) 订阅 /camera/color/image_raw/uncompressed + /camera/depth/image_rect_raw
        color_sub = message_filters.Subscriber("/camera/color/image_raw_uncompressed", Image)
        depth_sub = message_filters.Subscriber("/camera/depth/image_rect_raw", Image)
        sync = message_filters.ApproximateTimeSynchronizer([color_sub, depth_sub], queue_size=10, slop=7.5)
        sync.registerCallback(self.sync_callback)

        rospy.loginfo("Subscription set. Wait for color+depth.")

    def sync_callback(self, color_msg, depth_msg):
        color_img = self.bridge.imgmsg_to_cv2(color_msg, "bgr8")
        depth_img = self.bridge.imgmsg_to_cv2(depth_msg, "passthrough")
        if color_img is None or depth_img is None:
            return

        # (可选)尝试一些固定分辨率, e.g. 320×320, 416×416, etc. 避免 YuNet mismatch
        new_w, new_h = 640, 480
        resized_color = cv2.resize(color_img, (new_w,new_h))
        self.face_detector.setInputSize((new_w,new_h))

        result, faces = self.face_detector.detect(resized_color)
        faces = faces if faces is not None else []

        scale_x = color_img.shape[1]/float(new_w)
        scale_y = color_img.shape[0]/float(new_h)

        for face in faces:
            x,y,w,h = list(map(int, face[:4]))
            X = int(x*scale_x)
            Y = int(y*scale_y)
            W = int(w*scale_x)
            H = int(h*scale_y)

            center_x = X + W//2
            center_y = Y + H//2

            # depth
            depth_val = 0.0
            if 0 <= center_y < depth_img.shape[0] and 0 <= center_x < depth_img.shape[1]:
                zraw = depth_img[center_y, center_x]
                if depth_img.dtype == np.uint16:
                    depth_val = float(zraw)*0.001
                elif depth_img.dtype == np.float32:
                    depth_val = float(zraw)

            # 人脸对齐&识别
            aligned_face = self.face_recognizer.alignCrop(resized_color, face)
            feat = self.face_recognizer.feature(aligned_face)
            matched, (user_id, score) = match(self.face_recognizer, feat, self.dictionary)

            color_box = (0,255,0) if matched else (0,0,255)
            cv2.rectangle(color_img, (X,Y), (X+W,Y+H), color_box, 2)

            text_id = f"{user_id} ({score:.2f})"
            text_dp = f"Depth: {depth_val:.2f}m"
            cv2.putText(color_img, text_id, (X, Y-30), cv2.FONT_HERSHEY_SIMPLEX,0.6,color_box,2)
            cv2.putText(color_img, text_dp, (X, Y-10), cv2.FONT_HERSHEY_SIMPLEX,0.6,color_box,2)

        cv2.imshow("YuNet+SFace (ROS)", color_img)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            rospy.signal_shutdown("User pressed q")

def main():
    rospy.init_node("face_recognizer_node")

    # 改用 parse_known_args() 避免 __xxx:=xxx 报错
    parser = argparse.ArgumentParser()
    parser.add_argument("person", nargs="?", default="someone", help="Name for dictionary.")
    args, unknown = parser.parse_known_args()
    person_name = args.person

    node = YuNetFaceNode(person_name)
    rospy.spin()
    cv2.destroyAllWindows()

if __name__=="__main__":
    main()

