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
from std_msgs.msg import Float32MultiArray, String
from cv_bridge import CvBridge

COSINE_THRESHOLD = 0.38
PITCH_DEG        = 20.0
SLOP_SEC         = 0.3

WARMUP_FRAMES    = 7   # 初次检测到该人脸，先收集7帧再决定初始ID
SWITCH_FRAMES    = 7   # 多帧连续(7帧)不一致才切换ID

ID_OWNER    = 1
ID_STRANGER = 2

class FaceInfo:
    """
    记录某张脸的状态:
      - in_warmup: 是否还在预热阶段
      - warmup_count: 已收集多少帧
      - warmup_ids:   这几帧识别出的id
      - stable_id:    最终稳定ID(1/2)
      - mismatch_count: 多帧连续不一致计数
      - last_depth:   上一次成功的深度(避免抖到0)
    """
    def __init__(self):
        self.in_warmup       = True
        self.warmup_count    = 0
        self.warmup_ids      = []
        self.stable_id       = ID_STRANGER
        self.mismatch_count  = 0
        self.last_depth      = 0.0  # 用于保留上一帧深度

class FaceCenterDepthAlignedNode:
    def __init__(self, person_name):
        rospy.init_node("face_center_depth_warmup_filter", anonymous=True)
        self.bridge = CvBridge()

        #=== (A) 加载主人特征
        dictionary_dir = f"/home/niitsuma/catkin_ws/src/yunet/{person_name}/{person_name}_faces_dictionary"
        self.owner_feat = None
        self.have_owner = False
        if os.path.isdir(dictionary_dir):
            files = glob.glob(os.path.join(dictionary_dir, "*.npy"))
            if len(files)>0:
                self.owner_feat = np.load(files[0])
                self.have_owner = True
                rospy.loginfo(f"[WarmupFilter] 主人特征已加载: {files[0]}")
            else:
                rospy.logwarn("未找到主人特征npy文件")
        else:
            rospy.logwarn("主人特征文件夹不存在 => 无主人特征")

        #=== (B) YuNet + SFace模型
        model_dir = "/home/niitsuma/catkin_ws/src/yunet"
        det_onnx = f"{model_dir}/face_detection_yunet_2023mar.onnx"
        rec_onnx = f"{model_dir}/face_recognition_sface_2021dec.onnx"
        self.face_detector   = cv2.FaceDetectorYN_create(det_onnx, "", (640, 480))
        self.face_recognizer = cv2.FaceRecognizerSF_create(rec_onnx, "")

        #=== (C) 同步订阅 color + depth
        color_sub = message_filters.Subscriber(
            "/camera/color/image_raw_uncompressed", Image, queue_size=1)
        depth_sub = message_filters.Subscriber(
            "/camera/aligned_depth_to_color/image_raw", Image, queue_size=1)
        self.sync = message_filters.ApproximateTimeSynchronizer(
            [color_sub, depth_sub], queue_size=10, slop=SLOP_SEC)
        self.sync.registerCallback(self.sync_callback)

        #=== (D) 发布
        self.pub_face_info = rospy.Publisher("/face_depth_center", Float32MultiArray, queue_size=1)
        self.pub_face_id   = rospy.Publisher("/face_id_list", String, queue_size=1)

        #=== (E) face_map: {i: FaceInfo()}
        self.face_map = {}

        rospy.loginfo(f"[WarmupFilter] init done: WARMUP_FRAMES={WARMUP_FRAMES}, "
                      f"SWITCH_FRAMES={SWITCH_FRAMES}, COSINE_THRESHOLD={COSINE_THRESHOLD}")

    def sync_callback(self, color_msg, depth_msg):
        color_img = self.bridge.imgmsg_to_cv2(color_msg, "bgr8")
        depth_img = self.bridge.imgmsg_to_cv2(depth_msg, "passthrough")
        if color_img is None or depth_img is None:
            return

        #=== (1) 人脸检测
        self.face_detector.setInputSize((640,480))
        ret, faces = self.face_detector.detect(color_img)
        faces = faces if faces is not None else []

        face_data_list = []
        id_list = []
        current_idx = set()

        for i, face in enumerate(faces):
            x, y, w, h = face[:4].astype(int)
            cx = x + w//2
            cy = y + h//2
            current_idx.add(i)

            # 若本帧还没有 face_map[i], 则新建
            if i not in self.face_map:
                self.face_map[i] = FaceInfo()
            fi = self.face_map[i]

            #=== (2) 计算深度
            depth_val_los = self.get_depth_roi(depth_img, cx, cy, winsz=9)
            if depth_val_los<=0.0:
                # 若 ROI=无效, 用上帧深度
                depth_val_los = fi.last_depth
            else:
                # 若本帧有效, 更新last_depth
                fi.last_depth = depth_val_los

            # 再乘以 cos(俯仰角)
            depth_h = depth_val_los * math.cos(math.radians(PITCH_DEG))

            #=== (3) 人脸识别 => recognized_id
            aligned_face = self.face_recognizer.alignCrop(color_img, face)
            new_feat = self.face_recognizer.feature(aligned_face)
            sc_value = 0.0
            recognized_id = ID_STRANGER

            if self.have_owner and self.owner_feat is not None:
                sc = self.face_recognizer.match(
                    new_feat, self.owner_feat, cv2.FaceRecognizerSF_FR_COSINE)
                sc_value = sc
                if sc >= COSINE_THRESHOLD:
                    recognized_id = ID_OWNER

            #=== (4) Warmup
            if fi.in_warmup:
                fi.warmup_count += 1
                fi.warmup_ids.append(recognized_id)

                if fi.warmup_count >= WARMUP_FRAMES:
                    # 多数票
                    c1 = sum(1 for rid in fi.warmup_ids if rid==ID_OWNER)
                    c2 = sum(1 for rid in fi.warmup_ids if rid==ID_STRANGER)
                    if c1 >= c2:
                        fi.stable_id = ID_OWNER
                    else:
                        fi.stable_id = ID_STRANGER
                    fi.in_warmup = False
                    fi.warmup_ids.clear()
                    fi.warmup_count = 0

                # warmup期 不发布
                continue
            else:
                #=== (5) mismatch_count
                if recognized_id != fi.stable_id:
                    fi.mismatch_count += 1
                    if fi.mismatch_count >= SWITCH_FRAMES:
                        fi.stable_id = recognized_id
                        fi.mismatch_count = 0
                else:
                    fi.mismatch_count = 0

            final_id = fi.stable_id

            #=== (6) 画框
            cbox= (0,255,0) if final_id==ID_OWNER else (255,255,0)
            cv2.rectangle(color_img, (x,y), (x+w,y+h), cbox, 2)
            text_id = f"ID={final_id}, sc={sc_value:.2f}"
            cv2.putText(color_img, text_id, (x, y-35),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.6, cbox,2)
            text_dp = f"raw={depth_val_los:.2f} => H={depth_h:.2f}"
            cv2.putText(color_img, text_dp, (x, y-15),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0,255,255),2)

            #=== (7) 发布
            face_data_list.extend([float(cx), float(cy), depth_h])
            id_list.append(str(final_id))

        #=== 发布
        if len(face_data_list)>0:
            arr = Float32MultiArray()
            arr.data = face_data_list
            self.pub_face_info.publish(arr)
            self.pub_face_id.publish(",".join(id_list))

        #=== 清理未出现的 i
        to_remove = []
        for k in self.face_map.keys():
            if k not in current_idx:
                to_remove.append(k)
        for k in to_remove:
            self.face_map.pop(k)

        cv2.imshow("FaceCenterDepthAlignedNode_Warmup", color_img)
        if cv2.waitKey(1)&0xFF == ord('q'):
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
                return float(np.mean(vals))*0.001  # mm->m
            else:
                return float(np.mean(vals))
        return 0.0

def main():
    rospy.init_node("face_center_depth_warmup_filter", anonymous=True)
    parser= argparse.ArgumentParser()
    parser.add_argument("person", nargs="?", default="someone")
    args,_= parser.parse_known_args()

    node = FaceCenterDepthAlignedNode(args.person)
    rospy.spin()
    cv2.destroyAllWindows()

if __name__=="__main__":
    main()

