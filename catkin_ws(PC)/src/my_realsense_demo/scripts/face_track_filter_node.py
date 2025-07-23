#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
订阅
  /face_id_list        -> String        例 "1,2"
  /face_depth_center   -> Float32MultiArray  [cx,cy,d]×n
发布
  /face_id_list_stable
  /face_depth_center_stable
逻辑
  • 先用最近邻(基于 cx,cy)把 raw_ids 重排到与 stable 顺序对齐
  • 若排好序后与 stable_ids 不同:
        candidate_ids = 该序列
        candidate_count ++
        只要本帧序列与 candidate_ids 不同 => 重置 count
        连续 STABLE_N(9) 帧 全相同 才把 stable_ids 切换
  • 深度保护: d==0 或 |d-prev|>JUMP(0.8m) -> 用 prev
"""

import rospy, numpy as np
from std_msgs.msg import String, Float32MultiArray

STABLE_N = 7            # 连续 N 帧一致才切换
DIST_MAX = 1          # m, 最近邻阈值
JUMP_TH  = 1          # m, 深度跳变阈值

class SeqFilter:
    def __init__(self):
        rospy.init_node("face_id_sequence_filter")

        rospy.Subscriber("/face_id_list",      String, self.id_cb,    queue_size=1)
        rospy.Subscriber("/face_depth_center", Float32MultiArray, self.depth_cb, queue_size=1)

        self.pub_id    = rospy.Publisher("/face_id_list_stable", String,             queue_size=1)
        self.pub_depth = rospy.Publisher("/face_depth_center_stable", Float32MultiArray, queue_size=1)

        # 稳定状态
        self.stable_ids  = ""        # "1,2"
        self.stable_pts  = []        # [[cx,cy,d], ...]
        # 候选状态
        self.cand_ids    = ""
        self.cand_count  = 0

        self.cur_depth_arr = None

    # ---------------- 回调 ----------------
    def depth_cb(self,msg): self.cur_depth_arr = msg
    def id_cb(self,msg):
        if self.cur_depth_arr is None: return

        raw_ids = msg.data.strip()
        if not raw_ids: return
        raw_id_list = raw_ids.split(",")

        pts = self.parse_depth(self.cur_depth_arr.data)
        if len(pts) != len(raw_id_list): return

        # 首次初始化
        if self.stable_ids == "":
            self.stable_ids = raw_ids
            self.stable_pts = pts
            self.pub_all(self.stable_ids, self.stable_pts)
            return

        # 对齐顺序
        aligned_ids, aligned_pts = self.align_to_stable(raw_id_list, pts)
        seq = ",".join(aligned_ids)

        # 与 stable 相同 → 直接输出 + 重置候选
        if seq == self.stable_ids:
            self.cand_ids   = ""
            self.cand_count = 0
            self.stable_pts = self.smooth_depth(aligned_pts, self.stable_pts)
            self.pub_all(self.stable_ids, self.stable_pts)
            return

        # 与 stable 不同：
        if seq != self.cand_ids:
            # 新的候选序列
            self.cand_ids   = seq
            self.cand_count = 1
        else:
            self.cand_count += 1

        # 判断是否连续达到 STABLE_N
        if self.cand_count >= STABLE_N:
            self.stable_ids = self.cand_ids
            self.stable_pts = self.smooth_depth(aligned_pts, self.stable_pts)
            self.cand_ids   = ""
            self.cand_count = 0
            self.pub_all(self.stable_ids, self.stable_pts)
        else:
            # 仍未稳定 ⇒ 发布旧稳定值
            self.pub_all(self.stable_ids, self.stable_pts)

    # -------------- 工具函数 --------------
    @staticmethod
    def parse_depth(arr):
        return [[arr[i], arr[i+1], arr[i+2]] for i in range(0,len(arr),3)]

    def align_to_stable(self, raw_ids, raw_pts):
        """按照 stable_pts 的顺序，用最近距离把 raw 重新排序。"""
        new_ids, new_pts = [], []
        used=set()
        for sp in self.stable_pts:
            sx,sy,_ = sp
            best=-1; best_d=DIST_MAX
            for i,(cx,cy,_) in enumerate(raw_pts):
                if i in used: continue
                d=np.hypot(cx-sx, cy-sy)
                if d<best_d:
                    best_d=d; best=i
            if best>=0:
                used.add(best)
                new_ids.append(raw_ids[best])
                new_pts.append(raw_pts[best])
            else:
                # 如果没找到近的人脸，沿用旧 ID/旧坐标
                new_ids.append(self.stable_ids.split(",")[len(new_ids)])
                new_pts.append(sp)

        # 把剩余 raw 追加到末尾
        for i,(rid,pt) in enumerate(zip(raw_ids, raw_pts)):
            if i not in used:
                new_ids.append(rid)
                new_pts.append(pt)
        return new_ids,new_pts

    def smooth_depth(self, new_pts, prev_pts):
        out=[]
        for idx,(cx,cy,d_new) in enumerate(new_pts):
            if idx < len(prev_pts):
                _,_,d_prev = prev_pts[idx]
                if d_new==0 or abs(d_new-d_prev)>JUMP_TH:
                    out.append([cx,cy,d_prev])
                else:
                    out.append([cx,cy,d_new])
            else:
                out.append([cx,cy,d_new])
        return out

    def pub_all(self, ids, pts):
        depth_out=[]
        for cx,cy,d in pts:
            depth_out.extend([cx,cy,d])
        self.pub_id.publish(ids)
        self.pub_depth.publish(Float32MultiArray(data=depth_out))

def main():
    SeqFilter()
    rospy.spin()

if __name__=="__main__":
    main()

