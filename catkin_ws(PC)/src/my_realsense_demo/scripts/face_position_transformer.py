#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import rospy, math, time, numpy as np
import message_filters, tf
from std_msgs.msg import Float32MultiArray, String
from geometry_msgs.msg import PoseArray, Pose, Quaternion
from visualization_msgs.msg import Marker, MarkerArray
import tf.transformations as tft

LOST_TIMEOUT_SEC = 60.0        # 多久没更新就把那个人删掉

class FacePositionTransformer:
    def __init__(self):
        rospy.init_node("face_position_transformer", anonymous=False)

        # { pid: {"gx":..., "gy":..., "last_seen":...} }
        self.tracked_positions = {}

        self.tf_listener = tf.TransformListener()

        pos_sub = message_filters.Subscriber("/face_positions",     Float32MultiArray, queue_size=1)
        id_sub  = message_filters.Subscriber("/face_positions_id",  String,            queue_size=1)

        sync = message_filters.ApproximateTimeSynchronizer(
                    [pos_sub, id_sub], queue_size=10, slop=1.0, allow_headerless=True)
        sync.registerCallback(self.sync_callback)

        self.pose_pub   = rospy.Publisher("/face_global_positions",     PoseArray, queue_size=10)
        self.marker_pub = rospy.Publisher("/face_global_markers",       MarkerArray, queue_size=10)

        # ★ 新增：与 PoseArray 同时发布、同顺序的 pid 字符串
        self.id_pub     = rospy.Publisher("/face_global_positions_id",  String,     queue_size=10)   ### ←★


        rospy.Timer(rospy.Duration(0.1), self.publish_timer_cb)
        rospy.loginfo("face_position_transformer init, allow_headerless for /face_positions & /face_positions_id")

    # ------------------------------------------------------------------
    def sync_callback(self, pos_msg, id_msg):
        """
        把 base_link 坐标（x_base,y_base）转换到地图坐标 (gx,gy)，
        更新到 self.tracked_positions
        """
        data = pos_msg.data
        ids_str = id_msg.data
        if not data or not ids_str:
            rospy.loginfo("Empty pos or id => skip.")
            return

        if len(data) % 2 != 0:
            rospy.logwarn("face_positions length %d 不是 2 的倍数 => skip 尾巴 1.", len(data))
            data = data[:-1]

        pid_list = ids_str.split(",")
        coords_count = len(data) // 2
        if coords_count != len(pid_list):
            # 最多只处理两者都存在的那部分
            rospy.logwarn("coords_count=%d != id_count=%d => take min.",
                          coords_count, len(pid_list))
            coords_count = min(coords_count, len(pid_list))

        # === 取 map ← base_link 的 TF ===
        try:
            self.tf_listener.waitForTransform("map", "base_link",
                                              rospy.Time(0), rospy.Duration(0.2))
            trans, rot = self.tf_listener.lookupTransform("map", "base_link", rospy.Time(0))
        except Exception as e:
            rospy.logwarn(f"TF map->base_link fail: {e}")
            return

        M = tft.quaternion_matrix(rot)
        M[0:3, 3] = trans
        now_t = time.time()

        # === 更新/插入  ===
        idx = 0
        used_ids = []
        for i in range(coords_count):
            try:
                pid = int(pid_list[i])
            except ValueError:
                pid = 9999            # 异常占位

            x_base = data[idx]
            y_base = data[idx + 1]
            idx += 2

            base_vec = np.array([x_base, y_base, 0, 1], dtype=float)
            map_vec = M.dot(base_vec)
            gx, gy = map_vec[0], map_vec[1]

            self.tracked_positions[pid] = {"gx": gx,
                                           "gy": gy,
                                           "last_seen": now_t}
            used_ids.append(pid)

        # === 删除长时间未出现的 pid ===
        for pid in list(self.tracked_positions.keys()):
            if pid not in used_ids:
                if now_t - self.tracked_positions[pid]["last_seen"] > LOST_TIMEOUT_SEC:
                    rospy.loginfo(f"Remove ID={pid} not updated > {LOST_TIMEOUT_SEC}s")
                    self.tracked_positions.pop(pid)

    # ------------------------------------------------------------------
    def publish_timer_cb(self, event):
        """
        每 0.1 s 发布：
          • /face_global_positions     (PoseArray, map 坐标)
          • /face_global_markers       (RViz 文字)
          • /face_global_positions_id  (String，与 PoseArray 同顺序)   ### ←★
        """
        pose_arr = PoseArray()
        pose_arr.header.stamp = rospy.Time.now()
        pose_arr.header.frame_id = "map"

        marker_arr = MarkerArray()
        markers = []

        # ★ 按 pid 升序确保顺序确定、与 id_string 一致
        ordered_items = sorted(self.tracked_positions.items(), key=lambda kv: kv[0])   ### ←★
        id_string_parts = []

        for i, (pid, info) in enumerate(ordered_items):
            gx = info["gx"]
            gy = info["gy"]

            # PoseArray
            p = Pose()
            p.position.x = gx
            p.position.y = gy
            p.position.z = 0
            p.orientation = Quaternion(0, 0, 0, 1)
            pose_arr.poses.append(p)

            # Marker（可视化）
            mk = Marker()
            mk.header.frame_id = "map"
            mk.header.stamp = pose_arr.header.stamp
            mk.ns = "face_global"
            mk.id = i
            mk.type = Marker.TEXT_VIEW_FACING
            mk.action = Marker.ADD
            mk.pose.position.x = gx
            mk.pose.position.y = gy
            mk.pose.position.z = 0.15
            mk.pose.orientation.w = 1
            mk.scale.z = 0.2
            if pid == 1:          # 主人 → 绿色
                mk.color.r, mk.color.g, mk.color.b, mk.color.a = 0, 1, 0, 1
            else:                 # 陌生人 → 蓝色
                mk.color.r, mk.color.g, mk.color.b, mk.color.a = 0, 0, 1, 1
            mk.text = str(pid)
            mk.lifetime = rospy.Duration(0)
            markers.append(mk)

            id_string_parts.append(str(pid))    ### ←★ 记录顺序

        marker_arr.markers = markers

        # === 发布三条话题 ===
        self.pose_pub.publish(pose_arr)
        self.marker_pub.publish(marker_arr)
        self.id_pub.publish(String(data=",".join(id_string_parts)))   ### ←★

        rospy.loginfo_throttle(5,
            "Publishing %d faces in map." % len(ordered_items))

# ------------------------------------------------------------------
def main():
    FacePositionTransformer()
    rospy.spin()

if __name__ == "__main__":
    main()

