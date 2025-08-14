#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import sys
import time
import math
import glob
import argparse
import numpy as np
import cv2
import rospy
import message_filters
from sensor_msgs.msg import Image
from std_msgs.msg import Float32MultiArray, String
from cv_bridge import CvBridge

COSINE_THRESHOLD = 0.4   # 识别主人时用的阈值
PITCH_DEG = 20.0
SLOP_SEC = 0.3
LOST_TIMEOUT_SEC = 60.0   # 这里可以保留，也可以没用

class FaceCenterDepthAlignedNode:
    def __init__(self, person_name):
        rospy.loginfo(f"Initializing FaceCenterDepthAlignedNode with person={person_name}")
        self.bridge = CvBridge()

        #=== (A) 加载“主人”特征(若有)
        dictionary_dir = f"/home/niitsuma/catkin_ws/src/yunet/{person_name}/{person_name}_faces_dictionary"
        self.owner_feat = None
        self.have_owner = False
        if os.path.isdir(dictionary_dir):
            files = glob.glob(os.path.join(dictionary_dir, "*.npy"))
            if len(files)>0:
                feat = np.load(files[0])
                self.owner_feat = feat
                self.have_owner = True
                rospy.loginfo(f"主人特征已加载: {files[0]}")
            else:
                rospy.logwarn("未找到主人特征npy文件")
        else:
            rospy.logwarn("主人特征文件夹不存在 => 无主人特征")

        #=== 这里不需要再维护“陌生人库”了
        # self.tracked_strangers = {}
        # self.next_stranger_id = 2

        #=== (C) 加载YuNet + SFace模型
        model_dir = "/home/niitsuma/catkin_ws/src/yunet"
        det_onnx = os.path.join(model_dir, "face_detection_yunet_2023mar.onnx")
        rec_onnx = os.path.join(model_dir, "face_recognition_sface_2021dec.onnx")
        self.face_detector   = cv2.FaceDetectorYN_create(det_onnx, "", (640, 480))
        self.face_recognizer = cv2.FaceRecognizerSF_create(rec_onnx, "")

        #=== (D) 同步订阅 color + depth
        color_sub = message_filters.Subscriber("/camera/color/image_raw_uncompressed", Image, queue_size=1)
        depth_sub = message_filters.Subscriber("/camera/aligned_depth_to_color/image_raw", Image, queue_size=1)
        self.sync = message_filters.ApproximateTimeSynchronizer([color_sub, depth_sub],
                                                                queue_size=10,
                                                                slop=SLOP_SEC)
        self.sync.registerCallback(self.sync_callback)

        #=== (E) 发布  
        self.pub_face_info = rospy.Publisher("/face_depth_center", Float32MultiArray, queue_size=1)
        self.pub_face_id   = rospy.Publisher("/face_id_list", String, queue_size=1)

        rospy.loginfo(f"face_center_depth_aligned node init done. threshold={COSINE_THRESHOLD}, pitch={PITCH_DEG}deg, slop={SLOP_SEC}")

    def sync_callback(self, color_msg, depth_msg):
        color_img = self.bridge.imgmsg_to_cv2(color_msg, "bgr8")
        depth_img = self.bridge.imgmsg_to_cv2(depth_msg, "passthrough")
        if color_img is None or depth_img is None:
            return

        #=== 1) 人脸检测
        self.face_detector.setInputSize((640,480))
        ret, faces = self.face_detector.detect(color_img)
        faces = faces if faces is not None else []

        face_data_list = []
        id_list = []

        for face in faces:
            x, y, w, h = face[:4].astype(int)
            cx = x + w//2
            cy = y + h//2

            #=== (A) 计算 depth
            depth_val_los = self.get_depth_roi(depth_img, cx, cy, winsz=9)
            depth_h = depth_val_los * math.cos(math.radians(PITCH_DEG))

            #=== (B) 人脸识别 => 只有主人ID=1 or 陌生人ID=2
            aligned_face = self.face_recognizer.alignCrop(color_img, face)
            new_feat = self.face_recognizer.feature(aligned_face)
            final_id = 2      # 默认当成陌生人
            sc_value = 0.0    # 用来显示余弦相似度

            if self.have_owner and self.owner_feat is not None:
                sc = self.face_recognizer.match(new_feat, self.owner_feat, cv2.FaceRecognizerSF_FR_COSINE)
                sc_value = sc
                if sc>= COSINE_THRESHOLD:
                    final_id = 1    # 主人

            #=== (C) 在图像上画
            if final_id==1:
                color_box= (0,255,0)    # green
            else:
                color_box= (255,255,0)  # cyan(蓝+绿=青)

            cv2.rectangle(color_img, (x,y), (x+w,y+h), color_box, 2)
            text_id= f"ID={final_id}, sc={sc_value:.2f}"
            cv2.putText(color_img, text_id, (x, y-35), cv2.FONT_HERSHEY_SIMPLEX, 0.6, color_box,2)

            text_dp= f"raw={depth_val_los:.2f}m => H={depth_h:.2f}m"
            cv2.putText(color_img, text_dp, (x, y-15), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0,255,255),2)

            #=== (D) face_data_list + id_list
            face_data_list.extend([float(cx), float(cy), depth_h])
            id_list.append(str(final_id))

        #=== 如果有人 => 发布
        if len(face_data_list)>0:
            arr= Float32MultiArray()
            arr.data= face_data_list
            self.pub_face_info.publish(arr)
            self.pub_face_id.publish(",".join(id_list))

        #=== 显示
        cv2.imshow("FaceCenterDepthAlignedNode", color_img)
        if cv2.waitKey(1)&0xFF==ord('q'):
            rospy.signal_shutdown("User pressed q")

    def get_depth_roi(self, depth_img, cx, cy, winsz=9):
        r= winsz//2
        H,W= depth_img.shape[:2]
        y1= max(0, cy-r)
        y2= min(H, cy+r+1)
        x1= max(0, cx-r)
        x2= min(W, cx+r+1)
        roi= depth_img[y1:y2, x1:x2]
        mask= (roi>0)
        vals= roi[mask]
        if len(vals)>0:
            if depth_img.dtype==np.uint16:
                return float(np.mean(vals))*0.001
            else:
                return float(np.mean(vals))
        return 0.0

def main():
    rospy.init_node("face_center_depth_aligned", anonymous=True)
    parser= argparse.ArgumentParser()
    parser.add_argument("person", nargs="?", default="someone")
    args,_= parser.parse_known_args()

    node= FaceCenterDepthAlignedNode(args.person)
    rospy.spin()
    cv2.destroyAllWindows()

if __name__=="__main__":
    main()

