#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import rospy
import message_filters
import numpy as np
from sensor_msgs.msg import Image
from cv_bridge import CvBridge
import time

class DepthAlignNode:
    def __init__(self):
        self.bridge = CvBridge()
        self.busy = False  # 用于跳过回调

        #=== 1) 内/外参 (Depth->Color, 640×480) ===#
        # 参考 rs-enumerate-devices -c
        # Depth intrinsics
        self.fx_d = 387.971984863281
        self.fy_d = 387.971984863281
        self.cx_d = 319.639129638672
        self.cy_d = 235.604461669922
        # Color intrinsics
        self.fx_c = 604.643493652344
        self.fy_c = 604.55615234375
        self.cx_c = 325.292846679688
        self.cy_c = 253.172286987305
        # Extrinsics (Depth->Color)
        self.R = np.array([
            [0.999964,   0.00830885, -0.00167517],
            [-0.00831518, 0.999958,   -0.00380644],
            [0.00164347,  0.00382023,  0.999991 ]
        ], dtype=np.float64)
        self.T = np.array([0.0149952787, 0.0002010899, 0.0004349090], dtype=np.float64)

        self.depth_scale = 0.001  # mm->m

        #=== 2) 发布对齐后深度 ===#
        self.pub_aligned = rospy.Publisher(
            "/camera/aligned_depth_to_color/image_raw",
            Image, queue_size=1
        )

        #=== 3) 订阅 color & depth，设置 queue_size=1 避免堆积 ===#
        color_sub = message_filters.Subscriber(
            "/camera/color/image_raw_uncompressed", Image, queue_size=1)
        depth_sub = message_filters.Subscriber(
            "/camera/depth/image_rect_raw", Image, queue_size=1)

        # ApproximateTimeSynchronizer 也用 queue_size=1
        sync = message_filters.ApproximateTimeSynchronizer(
            [color_sub, depth_sub], queue_size=1, slop=1.0
        )
        sync.registerCallback(self.sync_callback)

        #=== 其它参数 ===#
        self.stride = 10  # 可以再调大
        self.frame_count = 0

        rospy.loginfo("[DepthAlignNode] Inited with stride=%d. queue=1 -> dropping old frames if busy", self.stride)

    def sync_callback(self, color_msg, depth_msg):
        # 如果上一次还没处理完，就直接跳过
        if self.busy:
            rospy.logwarn("Skipping a frame because still busy computing previous")
            return
        self.busy = True

        t0 = time.time()
        self.frame_count +=1
        rospy.loginfo(f"=== DepthAlign callback frame#{self.frame_count} ===")

        # 转OpenCV
        color_img = self.bridge.imgmsg_to_cv2(color_msg, "bgr8")
        depth_img = self.bridge.imgmsg_to_cv2(depth_msg, "passthrough")

        # 基本检查
        if color_img is None or depth_img is None:
            rospy.logwarn("color_img or depth_img is None => skip.")
            self.busy = False
            return
        h_c, w_c = color_img.shape[:2]
        h_d, w_d = depth_img.shape[:2]
        if (h_c!=480 or w_c!=640 or h_d!=480 or w_d!=640):
            rospy.logwarn("Expect 640x480 => skip. color=(%dx%d), depth=(%dx%d)", w_c,h_c,w_d,h_d)
            self.busy = False
            return

        aligned_depth = np.zeros((480,640), dtype=np.uint16)
        s = self.stride
        non_zero_in, non_zero_out = 0,0

        for vd in range(0,480,s):
            for ud in range(0,640,s):
                raw_depth = depth_img[vd, ud]
                if raw_depth==0:
                    continue
                non_zero_in +=1
                Zd = float(raw_depth)*self.depth_scale
                if Zd<=0:
                    continue

                Xd = (ud - self.cx_d)/self.fx_d * Zd
                Yd = (vd - self.cy_d)/self.fy_d * Zd
                Pd = np.array([Xd, Yd, Zd], dtype=np.float64)

                Pc = self.R.dot(Pd) + self.T
                Xc, Yc, Zc = Pc
                if Zc<=0:
                    continue

                uc = int(self.fx_c*(Xc/Zc) + self.cx_c)
                vc = int(self.fy_c*(Yc/Zc) + self.cy_c)
                if 0<=uc<640 and 0<=vc<480:
                    raw_depth_c = int(Zc/self.depth_scale)
                    # 用 stride×stride 来填充
                    for sub_v in range(vc, min(vc+s,480)):
                        for sub_u in range(uc, min(uc+s,640)):
                            aligned_depth[sub_v, sub_u] = raw_depth_c

        non_zero_out = np.count_nonzero(aligned_depth)

        # 发布
        aligned_msg = self.bridge.cv2_to_imgmsg(aligned_depth, encoding="16UC1")
        aligned_msg.header = color_msg.header
        self.pub_aligned.publish(aligned_msg)

        elapsed = time.time()-t0
        rospy.loginfo(f"[frame#{self.frame_count}] in_nonzero={non_zero_in}, out_nonzero={non_zero_out}, cost={elapsed:.3f}s")
        self.busy = False

def main():
    rospy.init_node("depth_align_node", anonymous=True)
    node = DepthAlignNode()
    rospy.spin()

if __name__=="__main__":
    main()

