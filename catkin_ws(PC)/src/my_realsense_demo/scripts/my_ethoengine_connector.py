#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
ROS  ->  EthoEngine  位姿透传节点
---------------------------------
  /osr_pose   (Float32MultiArray, 9 个浮点)  →  TCP (O,S,M　三条帧)

改动要点（# MOD 标注）:
1. 当 owner / stranger 没有被检测到时，不再发送默认 (0,1500)；
   而是把其坐标设置到地图范围外 (5000,5000,200)，
   EthoEngine 会据此判断该人物“当前不存在”。
2. 增加了 eps 阈值判断，防抖动 0.0 ± 1e‑6。
3. 其余接口、封包格式保持不变。
"""

import rospy
import socket
from std_msgs.msg import Float32MultiArray

# --------------------- EthoEngine 连接封装 ------------------------------
class EthoEngineConnector:
    def __init__(self, targethost="127.0.0.1", targetport=60000):
        self.eesocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.eesocket.connect((targethost, targetport))
        rospy.loginfo(f"[EthoEngineConnector] Connected to {targethost}:{targetport}")

    def __del__(self):
        self.eesocket.close()

    # ------------ 三类实体各自的发送（内部做边界裁剪） -------------
    def sendrobotpose(self, robot_pose):
        rp = [
            max(-3000, min(2999.9, robot_pose[0])),
            max(0,     min(3000,  robot_pose[1])),
            robot_pose[2]
        ]
        msg_str = f"M,{self.fmt(rp[0])},{self.fmt(rp[1])},{self.fmt(rp[2])},"
        self.eesocket.send(msg_str.encode('ascii'))

    def sendownerpose(self, owner_pose):
        op = [
            max(-3000, min(5000, owner_pose[0])),
            max(0,     min(4800, owner_pose[1])),
            owner_pose[2]
        ]
        msg_str = f"O,{self.fmt(op[0])},{self.fmt(op[1])},{self.fmt(op[2])},"
        self.eesocket.send(msg_str.encode('ascii'))

    def sendstrangerpose(self, stranger_pose):
        sp = [
            max(-3000, min(5000, stranger_pose[0])),
            max(0,     min(4800, stranger_pose[1])),
            stranger_pose[2]
        ]
        msg_str = f"S,{self.fmt(sp[0])},{self.fmt(sp[1])},{self.fmt(sp[2])},"
        self.eesocket.send(msg_str.encode('ascii'))

    def sendposes(self, owner_pose, stranger_pose, robot_pose):
        self.sendownerpose(owner_pose)
        self.sendstrangerpose(stranger_pose)
        self.sendrobotpose(robot_pose)

    # --- 数字转定长 7.1f 字符串 ---
    @staticmethod
    def fmt(num: float) -> str:
        return f"{num:07.1f}"


# --------------------- ROS 回调 ----------------------------------------
def osr_pose_callback(msg: Float32MultiArray):
    """
    data = [ owner_xyz , stranger_xyz , robot_xy_yaw ]  (共 9 个 float)
    单位: m -> 需转换为 EthoEngine 所用 mm，并加平面偏移。
    """
    if len(msg.data) < 9:
        rospy.logwarn("[my_ethoengine_connector] /osr_pose 长度不足 9, 丢弃")
        return

    eps = 1e-6                                # MOD  : 判断“0”的容忍误差
    # ---------- Owner ----------
    if abs(msg.data[0]) < eps and abs(msg.data[1]) < eps and abs(msg.data[2]) < eps:   # MOD
        owner_pose = [0.0, 0.0, 200.0]                                           # MOD
    else:
        owner_pose = [
            msg.data[0] * 1000,
            msg.data[1] * 1000 + 1500,
            msg.data[2] * 1000 + 200
        ]

    # ---------- Stranger ----------
    if abs(msg.data[3]) < eps and abs(msg.data[4]) < eps and abs(msg.data[5]) < eps:   # MOD
        stranger_pose = [0.0, 0.0, 200.0]                                        # MOD
    else:
        stranger_pose = [
            msg.data[3] * 1000,
            msg.data[4] * 1000 + 1500,
            msg.data[5] * 1000 + 200
        ]

    # ---------- Robot ----------
    robot_pose = [
        msg.data[6] * 1000,
        msg.data[7] * 1000 + 1500,
        msg.data[8]
    ]

    rospy.logdebug(  # 调试级别，避免刷屏
        f"[EE_conn] O={owner_pose}, S={stranger_pose}, M={robot_pose}"
    )
    eeconn.sendposes(owner_pose, stranger_pose, robot_pose)


# --------------------- 主入口 ------------------------------------------
if __name__ == "__main__":
    rospy.init_node("my_ethoengine_connector", anonymous=True)

    host = rospy.get_param("~ee_host", "127.0.0.1")
    port = rospy.get_param("~ee_port", 60000)

    eeconn = EthoEngineConnector(host, port)

    rospy.Subscriber("/osr_pose",
                     Float32MultiArray,
                     osr_pose_callback,
                     queue_size=1)

    rospy.loginfo("[my_ethoengine_connector] Ready.  Listening /osr_pose → EthoEngine")
    rospy.spin()

