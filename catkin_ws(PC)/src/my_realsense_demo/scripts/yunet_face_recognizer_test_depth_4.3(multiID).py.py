#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import sys
import time  # 关键：需导入time，否则NameError
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

COSINE_THRESHOLD = 0.4   # OpenCV推荐阈值
LOST_TIMEOUT_SEC = 60.0
PITCH_DEG = 20.0
SLOP_SEC = 0.3

class FaceCenterDepthAlignedNode:
    def __init__(self, person_name):
        rospy.loginfo("Initializing FaceCenterDepthAlignedNode with person=%s", person_name)
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
                rospy.loginfo("主人特征已加载: %s", files[0])
            else:
                rospy.logwarn("未找到主人特征npy文件")
        else:
            rospy.logwarn("主人特征文件夹不存在 => 无主人特征")

        #=== (B) 陌生人ID管理: { "2":{ "feature":..., "last_seen":... }, "3":..., ... }
        self.tracked_strangers = {}
        self.next_stranger_id = 2

        #=== (C) 加载YuNet + SFace模型
        model_dir = "/home/niitsuma/catkin_ws/src/yunet"
        det_onnx = os.path.join(model_dir, "face_detection_yunet_2023mar.onnx")
        rec_onnx = os.path.join(model_dir, "face_recognition_sface_2021dec.onnx")
        self.face_detector     = cv2.FaceDetectorYN_create(det_onnx,  "", (640, 480))
        self.face_recognizer   = cv2.FaceRecognizerSF_create(rec_onnx, "")

        #=== (D) 同步订阅 color + depth
        color_sub = message_filters.Subscriber("/camera/color/image_raw_uncompressed", Image, queue_size=1)
        depth_sub = message_filters.Subscriber("/camera/aligned_depth_to_color/image_raw", Image, queue_size=1)
        # slop=2.0=>能容忍较大时间差，避免回调不触发
        self.sync = message_filters.ApproximateTimeSynchronizer([color_sub, depth_sub],
                                                                queue_size=10,
                                                                slop=SLOP_SEC)
        self.sync.registerCallback(self.sync_callback)

        #=== (E) 发布: 
        #    /face_depth_center => [cx,cy,depthH, cx2,cy2,depthH2,...]
        #    /face_id_list => "1,2,2,3..."
        self.pub_face_info = rospy.Publisher("/face_depth_center", Float32MultiArray, queue_size=1)
        self.pub_face_id   = rospy.Publisher("/face_id_list", String, queue_size=1)

        rospy.loginfo("face_center_depth_aligned node init done. threshold=%.3f, pitch=%.1fdeg, slop=%.1f",
                      COSINE_THRESHOLD, PITCH_DEG, SLOP_SEC)

    def sync_callback(self, color_msg, depth_msg):
        color_img = self.bridge.imgmsg_to_cv2(color_msg, "bgr8")
        depth_img = self.bridge.imgmsg_to_cv2(depth_msg, "passthrough")
        if color_img is None or depth_img is None:
            return

        #=== 1) 人脸检测
        self.face_detector.setInputSize((640, 480))
        ret, faces = self.face_detector.detect(color_img)
        faces = faces if faces is not None else []

        face_data_list = []
        id_list = []

        for face in faces:
            x, y, w, h = face[:4].astype(int)
            cx = x + w//2
            cy = y + h//2

            # (A) ROI深度 => line_of_sight
            depth_val_los = self.get_depth_roi(depth_img, cx, cy, 9)
            # 修正成水平距离
            depth_h = depth_val_los * math.cos(math.radians(PITCH_DEG))

            # (B) 人脸识别 => 主人(ID=1) 或陌生人(2+)
            aligned_face = self.face_recognizer.alignCrop(color_img, face)
            new_feat = self.face_recognizer.feature(aligned_face)
            final_id = None

            # 先匹配主人
            if self.have_owner and self.owner_feat is not None:
                sc = self.face_recognizer.match(new_feat, self.owner_feat, cv2.FaceRecognizerSF_FR_COSINE)
                if sc >= COSINE_THRESHOLD:
                    final_id = 1

            # 若不是主人 => 匹配陌生人库
            if final_id is None:
                best_score=-1.0
                best_id=None
                for sid, info in self.tracked_strangers.items():
                    sc = self.face_recognizer.match(new_feat, info["feature"], cv2.FaceRecognizerSF_FR_COSINE)
                    if sc>best_score:
                        best_score= sc
                        best_id= sid
                if best_score>= COSINE_THRESHOLD and best_id is not None:
                    final_id= int(best_id)
                    self.tracked_strangers[best_id]["last_seen"] = time.time()
                    # 可选:更新特征
                    old_f= self.tracked_strangers[best_id]["feature"]
                    self.tracked_strangers[best_id]["feature"] = (old_f + new_feat)/2.0
                else:
                    # 新人
                    final_id= self.next_stranger_id
                    self.next_stranger_id+=1
                    self.tracked_strangers[str(final_id)] = {
                        "feature": new_feat,
                        "last_seen": time.time()
                    }

            # (C) 绘制方框 => ID=1 => 绿色, 否则 => 青色(bgr=(255,255,0))
            if final_id == 1:
                color_box = (0,255,0)   # green
            else:
                color_box = (255,255,0) # cyan

            cv2.rectangle(color_img, (x,y), (x+w,y+h), color_box, 2)
            # 在画面上打出 "ID=xx" + "raw= => H="
            text_id = f"ID={final_id}"
            text_dp = f"raw={depth_val_los:.2f}m => H={depth_h:.2f}m"
            cv2.putText(color_img, text_id, (x, y-35), cv2.FONT_HERSHEY_SIMPLEX, 0.6, color_box,2)
            cv2.putText(color_img, text_dp, (x, y-15), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0,255,255),2)

            raw_conf = face[4]
            score = 1.0 / (1.0 + math.exp(-raw_conf))  # Sigmoid
            cv2.putText(color_img, f"det={score:.2f}", 
                        (x, y-55),
                        cv2.FONT_HERSHEY_SIMPLEX, 
                        0.5, (0,255,255), 2)



            # 加入face_data_list & id_list
            face_data_list.extend([float(cx), float(cy), depth_h])
            id_list.append(str(final_id))

        # 如果检测到人 => 发布
        if len(face_data_list)>0:
            arr= Float32MultiArray()
            arr.data= face_data_list
            self.pub_face_info.publish(arr)
            self.pub_face_id.publish(",".join(id_list))

        # 清除失联陌生人
        self.prune_lost()

        # 显示
        cv2.imshow("FaceCenterDepthAlignedNode", color_img)
        if cv2.waitKey(1)&0xFF == ord('q'):
            rospy.signal_shutdown("User pressed q")

    def prune_lost(self):
        now_t= time.time()
        to_remove=[]
        for sid, info in self.tracked_strangers.items():
            if (now_t - info["last_seen"])> LOST_TIMEOUT_SEC:
                to_remove.append(sid)
        for sid in to_remove:
            rospy.loginfo(f"Remove stranger ID={sid}, not seen>60s")
            self.tracked_strangers.pop(sid)

    def get_depth_roi(self, depth_img, cx, cy, winsz=9):
        r= winsz//2
        H,W= depth_img.shape[:2]
        y1= max(0, cy-r)
        y2= min(H, cy+r+1)
        x1= max(0, cx-r)
        x2= min(W, cx+r+1)
        roi= depth_img[y1:y2, x1:x2]
        mask=(roi>0)
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

