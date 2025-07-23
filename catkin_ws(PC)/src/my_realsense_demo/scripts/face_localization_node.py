#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import math
import rospy
import message_filters
from std_msgs.msg import Float32MultiArray, String

class FaceLocalizationNode:
    def __init__(self):
        rospy.init_node("face_localization_node", anonymous=True)
        self.fov_deg = rospy.get_param("~camera_fov_deg", 70.0)
        self.img_width = rospy.get_param("~image_width", 640.0)
        self.cx_pix = 0.5*self.img_width
        self.robot_yaw_deg = rospy.get_param("~robot_yaw_deg", 0.0)

        depth_sub = message_filters.Subscriber("/face_depth_center_stable", Float32MultiArray, queue_size=1)
        id_sub    = message_filters.Subscriber("/face_id_list_stable", String, queue_size=1)
        # 重点: allow_headerless=True
        sync = message_filters.ApproximateTimeSynchronizer([depth_sub, id_sub],
                                                           queue_size=10,
                                                           slop=2.0,
                                                           allow_headerless=True)
        sync.registerCallback(self.sync_callback)

        self.pub_positions = rospy.Publisher("/face_positions", Float32MultiArray, queue_size=1)
        self.pub_positions_id = rospy.Publisher("/face_positions_id", String, queue_size=1)

        rospy.loginfo("FaceLocalizationNode init done. fov=%.1f, yaw=%.1f deg", self.fov_deg, self.robot_yaw_deg)

    def sync_callback(self, depth_msg, id_msg):
        data = depth_msg.data
        id_str = id_msg.data
        if not data or not id_str:
            rospy.loginfo("Empty face_depth_center or face_id_list => skip.")
            return
        n = len(data)
        if n%3!=0:
            rospy.logwarn("face_depth_center length %d not multiple of3 => skip."%n)
            return

        ids = id_str.split(",")
        face_count= n//3
        if face_count!= len(ids):
            rospy.logwarn("Mismatch face_count=%d vs id_count=%d => partial."%(face_count,len(ids)))
            face_count= min(face_count,len(ids))

        result_xy = []
        used_ids  = []
        idx=0
        for i in range(face_count):
            cx = data[idx]; cy = data[idx+1]; depthH= data[idx+2]
            idx+=3
            pid= ids[i]

            px_offset = cx - self.cx_pix
            deg_per_px= self.fov_deg / float(self.img_width)
            faceAngle_deg= self.robot_yaw_deg + px_offset*deg_per_px
            faceAngle_rad= math.radians(faceAngle_deg)
            face_x= depthH*math.cos(faceAngle_rad)
            face_y= - depthH*math.sin(faceAngle_rad)

            result_xy.append(face_x)
            result_xy.append(face_y)
            used_ids.append(pid)

        if len(result_xy)>0:
            arr_msg= Float32MultiArray()
            arr_msg.data= result_xy
            self.pub_positions.publish(arr_msg)
            self.pub_positions_id.publish(",".join(used_ids))

def main():
    node = FaceLocalizationNode()
    rospy.spin()

if __name__=="__main__":
    main()

