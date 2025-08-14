#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import rospy
import message_filters
import math
import time
import numpy as np
from std_msgs.msg import Float32MultiArray, String
from geometry_msgs.msg import PoseArray, Pose, Quaternion
from visualization_msgs.msg import Marker, MarkerArray
import tf

LOST_TIMEOUT_SEC = 60.0

class FacePositionTransformer:
    def __init__(self):
        rospy.init_node("face_position_transformer", anonymous=False)
        self.tracked_positions = {}  # { pid: {"gx":..., "gy":..., "last_seen":...}, ... }
        self.tf_listener = tf.TransformListener()

        pos_sub = message_filters.Subscriber("/face_positions", Float32MultiArray, queue_size=1)
        id_sub  = message_filters.Subscriber("/face_positions_id", String, queue_size=1)
        # 同样关键: allow_headerless=True
        sync = message_filters.ApproximateTimeSynchronizer([pos_sub, id_sub],
                                                           queue_size=10,
                                                           slop=1.0,
                                                           allow_headerless=True)
        sync.registerCallback(self.sync_callback)

        self.pose_pub   = rospy.Publisher("/face_global_positions", PoseArray, queue_size=10)
        self.marker_pub = rospy.Publisher("/face_global_markers", MarkerArray, queue_size=10)

        rospy.Timer(rospy.Duration(0.1), self.publish_timer_cb)
        rospy.loginfo("face_position_transformer init, allow_headerless for /face_positions & /face_positions_id")

    def sync_callback(self, pos_msg, id_msg):
        data = pos_msg.data
        ids_str = id_msg.data
        if not data or not ids_str:
            rospy.loginfo("Empty pos or id => skip.")
            return
        n= len(data)
        if n%2!=0:
            rospy.logwarn("face_positions length %d not multiple of2 => skip 1."%n)
            n-=1
        coords_count= n//2
        pid_list= ids_str.split(",")
        if coords_count!= len(pid_list):
            rospy.logwarn("Mismatch coords_count=%d vs id_count=%d => partial."%(coords_count,len(pid_list)))
            coords_count= min(coords_count, len(pid_list))

        try:
            self.tf_listener.waitForTransform("map","base_link", rospy.Time(0), rospy.Duration(0.2))
            (trans,rot)= self.tf_listener.lookupTransform("map","base_link", rospy.Time(0))
        except Exception as e:
            rospy.logwarn(f"TF map->base_link fail => skip. {e}")
            return

        import tf.transformations as tft
        M= tft.quaternion_matrix(rot)
        M[0:3,3]= trans
        now_t= time.time()

        used_ids=[]
        idx=0
        for i in range(coords_count):
            pid_str= pid_list[i]
            try:
                pid= int(pid_str)
            except:
                pid=9999
            x_base= data[idx]; y_base= data[idx+1]; idx+=2

            base_vec= np.array([x_base,y_base,0,1], dtype=float)
            map_vec= M.dot(base_vec)
            gx,gy,gz= map_vec[0], map_vec[1], map_vec[2]
            gz=0.0

            self.tracked_positions[pid]= {
                "gx":gx, "gy":gy, "last_seen":now_t
            }
            used_ids.append(pid)

        # 清理超过60秒
        to_remove=[]
        for pid,info in self.tracked_positions.items():
            if pid not in used_ids:
                if (now_t - info["last_seen"])> LOST_TIMEOUT_SEC:
                    to_remove.append(pid)
        for pid in to_remove:
            rospy.loginfo(f"Remove ID={pid} not updated>60s")
            self.tracked_positions.pop(pid)

    def publish_timer_cb(self, event):
        pose_arr= PoseArray()
        pose_arr.header.stamp= rospy.Time.now()
        pose_arr.header.frame_id= "map"

        marker_arr= MarkerArray()
        markers=[]
        i=0
        for pid,info in self.tracked_positions.items():
            gx= info["gx"]
            gy= info["gy"]

            p= Pose()
            p.position.x= gx
            p.position.y= gy
            p.position.z=0
            p.orientation= Quaternion(0,0,0,1)
            pose_arr.poses.append(p)

            mk= Marker()
            mk.header.frame_id="map"
            mk.header.stamp= pose_arr.header.stamp
            mk.ns="face_global"
            mk.id=i
            i+=1
            mk.type= Marker.TEXT_VIEW_FACING
            mk.action= Marker.ADD
            mk.pose.position.x= gx
            mk.pose.position.y= gy
            mk.pose.position.z= 0.15
            mk.pose.orientation.w=1
            mk.scale.z=0.2
            if pid==1:
                # 主人 => 绿色
                mk.color.r=0; mk.color.g=1; mk.color.b=0; mk.color.a=1
            else:
                # 陌生人 => 蓝
                mk.color.r=0; mk.color.g=0; mk.color.b=1; mk.color.a=1
            mk.text= str(pid)
            mk.lifetime= rospy.Duration(0)
            markers.append(mk)

        marker_arr.markers= markers
        self.pose_pub.publish(pose_arr)
        self.marker_pub.publish(marker_arr)
        rospy.loginfo_throttle(5,"Publishing %d faces in map."%len(self.tracked_positions))

def main():
    rospy.init_node("face_position_transformer", anonymous=False)
    node= FacePositionTransformer()
    rospy.spin()

if __name__=="__main__":
    main()
