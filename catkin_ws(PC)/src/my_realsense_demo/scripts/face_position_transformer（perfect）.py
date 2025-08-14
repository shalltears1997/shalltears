#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
本节点用于将 /face_positions（base_link坐标系下的人位置列表）
转换到 map 坐标系，并周期性地发布到 /face_global_positions (PoseArray)。

订阅:
  /face_positions (std_msgs/Float32MultiArray)
    - 数据格式: [x1, y1, x2, y2, ...]
    - 表示在 base_link 坐标系下的多人2D位置
发布:
  /face_global_positions (geometry_msgs/PoseArray)
    - frame_id = "map"
    - pose.position.z = 0, pose.orientation = (0,0,0,1)
TF 需求:
  - map -> odom -> base_link 要可用
    例如SLAM或AMCL发布 map->odom, 里程计或底盘发布 odom->base_link

周期：
  - 每0.1秒(10Hz)发布一次PoseArray，即使没有新的检测或transform失败
"""

import rospy
import tf
import numpy as np
from std_msgs.msg import Float32MultiArray
from geometry_msgs.msg import PoseArray, Pose, Point, Quaternion

class FacePositionTransformer:
    def __init__(self):
        rospy.init_node("face_position_transformer", anonymous=False)

        # TF监听器：查找 base_link -> map 的坐标变换
        self.tf_listener = tf.TransformListener()

        # 发布：全局坐标下的人位置 => PoseArray
        self.pose_pub = rospy.Publisher(
            "/face_global_positions",
            PoseArray,
            queue_size=10
        )

        # 存储最新/最后一次转换成功的人全局坐标列表
        # 格式: [(x, y, z), (x, y, z), ...] (map坐标系)
        self.last_global_positions = []

        # 标志：是否已收到过检测结果
        self.received_detection = False

        # 订阅本地人位置（相对base_link）
        rospy.Subscriber(
            "/face_positions",
            Float32MultiArray,
            self.detections_callback,
            queue_size=1
        )

        # 定时器：10Hz 发布
        rospy.Timer(rospy.Duration(0.1), self.publish_timer_callback)

        rospy.loginfo("face_position_transformer 初始化完成，等待 /face_positions 并将结果转换到 map.")

    def detections_callback(self, msg):
        """
        当检测到新的人脸位置时会调用本回调。
        msg.data => [x1, y1, x2, y2, ...] (base_link坐标)
        将其转换到 map，并更新 self.last_global_positions。
        如果 transform 失败，则保留上次不变。
        """
        data = msg.data
        num_vals = len(data)
        if num_vals == 0:
            # 当前帧未检测到人 => 不更新，保留上一次
            rospy.loginfo("当前 /face_positions 无检测结果，保留旧坐标。")
            self.received_detection = True
            return

        # 若数量不是偶数(必须成对 x,y)
        if num_vals % 2 != 0:
            rospy.logwarn("接收到 /face_positions 长度=%d，不是(x,y)对的倍数，忽略最后一个值。" % num_vals)
            num_vals -= 1  # 忽略最后一个没法配对的值

        # 尝试查找 base_link -> map 的变换
        try:
            (trans, rot) = self.tf_listener.lookupTransform(
                "map",       # 目标坐标系
                "base_link", # 源坐标系
                rospy.Time(0)
            )
        except (tf.LookupException,
                tf.ConnectivityException,
                tf.ExtrapolationException) as e:
            rospy.logwarn("无法从 base_link 转换到 map: %s. 本帧坐标不更新." % str(e))
            return

        # 解出平移和四元数
        tx, ty, tz = trans
        qx, qy, qz, qw = rot

        # 将四元数转换为旋转矩阵 (3x3)
        # 公式参考： https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles#Rotation_matrices
        x2, y2, z2 = qx*qx, qy*qy, qz*qz
        xy, xz, yz = qx*qy, qx*qz, qy*qz
        wx, wy, wz = qw*qx, qw*qy, qw*qz
        rot_matrix = np.array([
            [1 - 2*(y2+z2), 2*(xy - wz),     2*(xz + wy)],
            [2*(xy + wz),   1 - 2*(x2+z2),   2*(yz - wx)],
            [2*(xz - wy),   2*(yz + wx),     1 - 2*(x2 + y2)]
        ])

        new_global_positions = []
        # 将检测到的 (x_local, y_local) 批量转换到 map
        for i in range(0, num_vals, 2):
            x_local = data[i]
            y_local = data[i+1]
            z_local = 0.0

            local_vec = np.array([x_local, y_local, z_local])
            # 先旋转
            rotated = rot_matrix.dot(local_vec)
            # 再平移
            gx = rotated[0] + tx
            gy = rotated[1] + ty
            gz = rotated[2] + tz

            # 这里可选：强制 gz=0，表示只做平面化
            gz = 0.0

            new_global_positions.append((gx, gy, gz))

        self.last_global_positions = new_global_positions
        self.received_detection = True
        rospy.loginfo("本帧成功转换 %d 个点到 map 坐标系." % (len(new_global_positions)))

    def publish_timer_callback(self, event):
        """
        每0.1秒(10Hz)执行一次，用上次记录的全局坐标集合来发布PoseArray。
        如果还没收到任何检测，则不发布。
        """
        if not self.received_detection:
            # 尚未收到任何数据
            return

        # 构造 PoseArray
        pose_array = PoseArray()
        pose_array.header.stamp = rospy.Time.now()
        pose_array.header.frame_id = "map"

        for (gx, gy, gz) in self.last_global_positions:
            pose = Pose()
            pose.position = Point(gx, gy, gz)
            # 不关心朝向，设置为单位四元数
            pose.orientation = Quaternion(0, 0, 0, 1)
            pose_array.poses.append(pose)

        self.pose_pub.publish(pose_array)
        # 可以用 throttle 函数避免太多日志
        rospy.loginfo_throttle(1.0, "发布 %d 个点到 /face_global_positions" % len(pose_array.poses))

def main():
    try:
        node = FacePositionTransformer()
        rospy.spin()
    except rospy.ROSInterruptException:
        pass

if __name__ == "__main__":
    main()
