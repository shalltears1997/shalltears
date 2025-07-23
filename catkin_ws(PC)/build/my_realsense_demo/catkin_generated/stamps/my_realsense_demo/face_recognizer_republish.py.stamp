#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
import glob
import argparse
import numpy as np
import cv2
import rospy
import message_filters
from sensor_msgs.msg import Image
from cv_bridge import CvBridge

COSINE_THRESHOLD = 0.363

def match_feature(recognizer, feature1, dictionary):
    for (user_id, feature2) in dictionary:
        score = recognizer.match(feature1, feature2, cv2.FaceRecognizerSF_FR_COSINE)
        if score > COSINE_THRESHOLD:
            return True, (user_id, score)
    return False, ("unknown", 0.0)

class YUNetFaceRecRepublish:
    def __init__(self, person_name):
        rospy.loginfo("Initializing YUNetFaceRecRepublish node...")

        # 1) 模型和特征路径
        self.model_dir = "/home/niitsuma/catkin_ws/src/yunet"
        self.dictionary_dir = os.path.join(self.model_dir, person_name, f"{person_name}_faces_dictionary")

        # 2) 加载dictionary
        self.dictionary = []
        files = glob.glob(os.path.join(self.dictionary_dir, "*.npy"))
        for file in files:
            feat = np.load(file)
            uid = os.path.splitext(os.path.basename(file))[0]
            self.dictionary.append((uid, feat))
        rospy.loginfo(f"Loaded {len(self.dictionary)} face features from {self.dictionary_dir}")

        # 3) 加载 YuNet + SFace
        detector_onnx = os.path.join(self.model_dir, "face_detection_yunet_2023mar.onnx")
        recognizer_onnx = os.path.join(self.model_dir, "face_recognition_sface_2021dec.onnx")

        self.face_detector = cv2.FaceDetectorYN_create(detector_onnx, "", (0,0))
        self.face_recognizer = cv2.FaceRecognizerSF_create(recognizer_onnx, "")

        self.bridge = CvBridge()

        # 4) 同步订阅
        color_sub = message_filters.Subscriber("/camera/color/image_raw_uncompressed", Image)
        depth_sub = message_filters.Subscriber("/camera/depth/image_rect_raw", Image)

        # Approx. sync color + depth
        self.sync = message_filters.ApproximateTimeSynchronizer([color_sub, depth_sub], queue_size=10, slop=0.1)
        self.sync.registerCallback(self.sync_callback)

        rospy.loginfo("Subscription set up. Subscribing color=..._uncompressed, depth=..._raw")

    def sync_callback(self, color_msg, depth_msg):
        color_img = self.bridge.imgmsg_to_cv2(color_msg, desired_encoding="bgr8")
        if color_img is None:
            rospy.logwarn("color_img is None.")
            return

        # depth raw => "passthrough" or "mono16" if it's 16UC1
        depth_img = self.bridge.imgmsg_to_cv2(depth_msg, desired_encoding="passthrough")
        if depth_img is None:
            rospy.logwarn("depth_img is None.")
            return

        h, w, _ = color_img.shape
        self.face_detector.setInputSize((w, h))

        # 人脸检测
        ret, faces = self.face_detector.detect(color_img)
        faces = faces if faces is not None else []

        for face in faces:
            x, y, fw, fh = list(map(int, face[:4]))
            center_x = x + fw//2
            center_y = y + fh//2

            # 取深度
            if 0 <= center_y < depth_img.shape[0] and 0 <= center_x < depth_img.shape[1]:
                depth_value = depth_img[center_y, center_x]
            else:
                depth_value = 0

            # 提取特征
            aligned_face = self.face_recognizer.alignCrop(color_img, face)
            feat = self.face_recognizer.feature(aligned_face)

            found, (uid, score) = match_feature(self.face_recognizer, feat, self.dictionary)
            color_box = (0,255,0) if found else (0,0,255)
            cv2.rectangle(color_img, (x,y), (x+fw, y+fh), color_box, 2)

            txt_id = f"{uid} ({score:.2f})"
            txt_depth = f"Depth: {depth_value:.2f}"
            cv2.putText(color_img, txt_id, (x, y-30), cv2.FONT_HERSHEY_SIMPLEX, 0.6, color_box, 2)
            cv2.putText(color_img, txt_depth, (x, y-10), cv2.FONT_HERSHEY_SIMPLEX, 0.6, color_box, 2)

        cv2.imshow("Republish Demo - color=uncompressed, depth=raw", color_img)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            rospy.signal_shutdown("User pressed q in OpenCV window.")

def main():
    rospy.init_node("face_recognizer_republish", anonymous=True)

    # 解析person
    parser = argparse.ArgumentParser()
    parser.add_argument("person", nargs="?", default="testperson",
                        help="Dictionary folder name. e.g. tarou")
    args, unknown = parser.parse_known_args()
    person_name = args.person

    node = YUNetFaceRecRepublish(person_name)
    rospy.spin()
    cv2.destroyAllWindows()

if __name__=="__main__":
    main()
