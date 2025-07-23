#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import math
import rospy
from std_msgs.msg import Float32MultiArray
from std_msgs.msg import Float32MultiArray

class FaceLocalizationNode:
    def __init__(self):
        # 读取参数：相机水平视场角(度)、图像宽度/中心像素、机器人自身yaw等
        self.fov_deg   = rospy.get_param("~camera_fov_deg", 70.0)  # 相机水平 FOV(度)
        self.img_width = rospy.get_param("~image_width",   640)
        # 例如 640 宽 => 中心 cx=320 (若无畸变时)
        self.cx_pix    = 0.5 * self.img_width

        # 机器人的朝向(度)（简单起见，用固定值/参数）
        self.robot_yaw_deg = rospy.get_param("~robot_yaw_deg", 0.0)

        # 订阅 /face_depth_center
        self.sub_face_depth = rospy.Subscriber(
            "/face_depth_center", 
            Float32MultiArray, 
            self.face_depth_cb,
            queue_size=1
        )
        
        # 发布 /face_positions
        self.pub_positions = rospy.Publisher(
            "/face_positions",
            Float32MultiArray,
            queue_size=1
        )

        rospy.loginfo("FaceLocalizationNode init done. fov_deg=%.1f, robotYaw=%.1f deg",
                      self.fov_deg, self.robot_yaw_deg)

    def face_depth_cb(self, msg):
        """
        msg.data: [cx1, cy1, depthH1,  cx2, cy2, depthH2, ...]
        其中 depthH 已是水平方向的距离(考虑了相机俯仰20°校正).
        我们还需要根据(cx - cx_pix)去计算水平角度theta，然后
        totalAngle = robotYaw + theta，再求 x,y = depthH * [cos, sin].
        """
        data = msg.data
        n = len(data)
        if n % 3 != 0:
            rospy.logwarn("face_depth_center length = %d not multiple of 3!", n)
            return

        # 结果 (x,y) 列表
        result_xy = []

        # 每个人脸 3个float: [cx, cy, depthH]
        # 这里 cy 暂时不使用(除非你要竖直方向做别的)
        for i in range(0, n, 3):
            cx = data[i+0]  # 像素 x
            # cy = data[i+1] # 像素 y (我们不使用)
            depthH = data[i+2]  # 水平距离

            # 计算图像中心偏移
            # px_offset: (cx - cx_pix), 若cx_pix=320 => 320→像素中心
            px_offset = cx - self.cx_pix
            # 多少度 / 每像素
            deg_per_px = self.fov_deg / float(self.img_width)
            # faceAngle(度) = px_offset * deg_per_px
            faceAngle_deg = px_offset * deg_per_px
            # 机器人自身朝向 + faceAngle
            totalAngle_deg = self.robot_yaw_deg + faceAngle_deg
            # 转弧度
            totalAngle_rad = math.radians(totalAngle_deg)

            # 计算 face 在机器人坐标系下的 x,y (2D)
            face_x = depthH * math.cos(totalAngle_rad)
            face_y = - depthH * math.sin(totalAngle_rad)

            result_xy.append(face_x)
            result_xy.append(face_y)

        # 发布
        out_msg = Float32MultiArray()
        out_msg.data = result_xy  # [x1,y1, x2,y2, ...]
        self.pub_positions.publish(out_msg)

def main():
    rospy.init_node("face_localization_node", anonymous=True)
    node = FaceLocalizationNode()
    rospy.spin()

if __name__ == "__main__":
    main()
