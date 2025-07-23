#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import sys
import math
import glob
import argparse
import numpy as np
import cv2
import rospy
import message_filters

from sensor_msgs.msg import Image
from std_msgs.msg import Float32MultiArray
from cv_bridge import CvBridge


COSINE_THRESHOLD = 0.363


def match_feature(recognizer, feature1, dictionary):
    """使用SFace特征进行匹配，超过阈值则返回 matched=True, 并给出 (user_id, score)。"""
    for (user_id, feature2) in dictionary:
        score = recognizer.match(feature1, feature2, cv2.FaceRecognizerSF_FR_COSINE)
        if score > COSINE_THRESHOLD:
            return True, (user_id, score)
    return False, ("unknown", 0.0)


class FaceCenterDepthAlignedNode:
    def __init__(self, person_name):
        rospy.loginfo("Initializing FaceCenterDepthAlignedNode for person=%s", person_name)
        self.bridge = CvBridge()

        #=== (1) 加载 SFace 字典特征 ===#
        dictionary_dir = f"/home/niitsuma/catkin_ws/src/yunet/{person_name}/{person_name}_faces_dictionary"
        self.dictionary = []
        files = glob.glob(os.path.join(dictionary_dir, "*.npy"))
        for file in files:
            feat = np.load(file)
            uid = os.path.splitext(os.path.basename(file))[0]
            self.dictionary.append((uid, feat))
        rospy.loginfo("Loaded %d face features from %s", len(self.dictionary), dictionary_dir)

        #=== (2) 加载 YuNet + SFace 模型 ===#
        model_dir = "/home/niitsuma/catkin_ws/src/yunet"
        detector_onnx  = os.path.join(model_dir, "face_detection_yunet_2023mar.onnx")
        recognizer_onnx= os.path.join(model_dir, "face_recognition_sface_2021dec.onnx")

        self.face_detector     = cv2.FaceDetectorYN_create(detector_onnx,  "", (640, 480))
        self.face_recognizer   = cv2.FaceRecognizerSF_create(recognizer_onnx,  "")

        #=== (3) 同步订阅 color + aligned_depth (640x480) ===#
        color_sub = message_filters.Subscriber("/camera/color/image_raw_uncompressed", Image, queue_size=1)
        depth_sub = message_filters.Subscriber("/camera/aligned_depth_to_color/image_raw", Image, queue_size=1)

        sync = message_filters.ApproximateTimeSynchronizer([color_sub, depth_sub],
                                                           queue_size=10, slop=0.5)
        sync.registerCallback(self.sync_callback)

        rospy.loginfo("Subscribed color+aligned_depth @640x480. Doing ROI-average, plus 20deg correction...")

        #=== (4) 额外发布一个话题: /face_depth_center (Float32MultiArray) ===#
        #     每帧包含多个人脸信息 => [cx, cy, depth_los, depth_h,  cx2,cy2,depth_los2,depth_h2, ...]
        self.pub_face_info = rospy.Publisher("/face_depth_center", Float32MultiArray, queue_size=1)

        #=== (5) 设置相机倾斜角(20度) => cos(20°) 用来修正深度 => 水平距离
        self.pitch_deg = 20.0

    def sync_callback(self, color_msg, depth_msg):
        # 1) 转成 OpenCV
        color_img = self.bridge.imgmsg_to_cv2(color_msg,  "bgr8")
        depth_img = self.bridge.imgmsg_to_cv2(depth_msg, "passthrough")
        if color_img is None or depth_img is None:
            return

        # 2) 人脸检测
        ret, faces = self.face_detector.detect(color_img)
        faces = faces if faces is not None else []

        # 准备发布的数组: [cx, cy, depthLOS, depthHoriz,  cx2,cy2,depthLOS2, depthHoriz2, ...]
        face_data_list = []

        for face in faces:
            x, y, w, h = list(map(int, face[:4]))
            cx = x + w//2
            cy = y + h//2

            #=== (A) ROI邻域取平均深度 => 视线距离 line_of_sight ===#
            depth_val_los = 0.0
            winsz = 9  # 9x9
            r = winsz//2
            H, W_ = depth_img.shape[:2]
            y1 = max(0, cy-r)
            y2 = min(H, cy+r+1)
            x1 = max(0, cx-r)
            x2 = min(W_, cx+r+1)
            roi = depth_img[y1:y2, x1:x2]

            valid_mask = (roi>0)
            valid_vals = roi[valid_mask]
            if len(valid_vals)>0:
                if depth_img.dtype == np.uint16:
                    # mm => m
                    depth_val_los = float(np.mean(valid_vals))*0.001
                elif depth_img.dtype == np.float32:
                    depth_val_los = float(np.mean(valid_vals))

            #=== (B) 倾斜 20° => 计算水平距离 (depth_h) = line_of_sight * cos(20°) ===#
            depth_h = depth_val_los * math.cos(math.radians(self.pitch_deg))

            #=== (C) 人脸识别 ===#
            aligned_face = self.face_recognizer.alignCrop(color_img, face)
            feat = self.face_recognizer.feature(aligned_face)
            matched, (user_id, user_score) = match_feature(self.face_recognizer, feat, self.dictionary)

            #=== (D) 可视化: 在图像上画框 + 显示 => "LoS=..., H=..." ===#
            color_box = (0,255,0) if matched else (0,0,255)
            cv2.rectangle(color_img, (x,y), (x+w,y+h), color_box, 2)
            cv2.circle(color_img, (cx,cy), 3, (0,255,255), -1)

            text_id = f"{user_id}({user_score:.2f})"
            text_dp = f"raw={depth_val_los:.2f}m => H={depth_h:.2f}m"
            cv2.putText(color_img, text_id, (x, y-35),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.6, color_box, 2)
            cv2.putText(color_img, text_dp, (x, y-15),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0,255,255), 2)

            #=== (E) face_data_list append => [cx, cy, depthLoS, depthH] ===#
            face_data_list.append(float(cx))
            face_data_list.append(float(cy))
            face_data_list.append(depth_h)

        #=== (F) 若本帧检测到人脸 => 发布 /face_depth_center ===#
        if len(face_data_list) > 0:
            arr_msg = Float32MultiArray()
            arr_msg.data = face_data_list
            self.pub_face_info.publish(arr_msg)

        #=== (G) 显示 ===#
        cv2.imshow("FaceCenterDepthAlignedNode", color_img)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            rospy.signal_shutdown("User pressed q")

def main():
    rospy.init_node("face_center_depth_aligned", anonymous=True)
    parser = argparse.ArgumentParser()
    parser.add_argument("person", nargs="?", default="someone",
                        help="Which dictionary to load for SFace features")
    args, _ = parser.parse_known_args()

    node = FaceCenterDepthAlignedNode(args.person)
    rospy.spin()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()

