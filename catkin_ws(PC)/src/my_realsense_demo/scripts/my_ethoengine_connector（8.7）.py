#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import rospy
import time
import socket
from std_msgs.msg import Float32MultiArray

class EthoEngineConnector:
    def __init__(self, targethost="127.0.0.1", targetport=60000):
        self.eesocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.eesocket.connect((targethost, targetport))
        rospy.loginfo(f"[EthoEngineConnector] Connected to {targethost}:{targetport}")

    def __del__(self):
        self.eesocket.close()

    def sendrobotpose(self, robot_pose):
        # bounding X in [-3000..2999.9], Y in [0..3000]
        rp= [
            max(-3000,min(2999.9,robot_pose[0])),
            max(0,     min(3000,  robot_pose[1])),
            robot_pose[2]
        ]
        msg_str= f"M,{self.format_posestr(rp[0])},{self.format_posestr(rp[1])},{self.format_posestr(rp[2])},"
        self.eesocket.send(msg_str.encode('ascii'))

    def sendownerpose(self, owner_pose):
        # bounding X in [-3000..+5000], Y in [0..4800]
        op= [
            max(-3000,min(5000,owner_pose[0])),
            max(0,     min(4800,owner_pose[1])),
            owner_pose[2]
        ]
        msg_str= f"O,{self.format_posestr(op[0])},{self.format_posestr(op[1])},{self.format_posestr(op[2])},"
        self.eesocket.send(msg_str.encode('ascii'))

    def sendstrangerpose(self, stranger_pose):
        sp= [
            max(-3000,min(5000,stranger_pose[0])),
            max(0,     min(4800,stranger_pose[1])),
            stranger_pose[2]
        ]
        msg_str= f"S,{self.format_posestr(sp[0])},{self.format_posestr(sp[1])},{self.format_posestr(sp[2])},"
        self.eesocket.send(msg_str.encode('ascii'))

    def sendposes(self, owner_pose, stranger_pose, robot_pose):
        self.sendownerpose(owner_pose)
        self.sendstrangerpose(stranger_pose)
        self.sendrobotpose(robot_pose)

    def format_posestr(self, num):
        return f"{num:07.1f}"

def osr_pose_callback(msg):
    """
    msg => Float32MultiArray => data = [9 floats]
    结构:
      idx0=owner_x, idx1=owner_y, idx2=owner_z,
      idx3=stranger_x, idx4=stranger_y, idx5=stranger_z,
      idx6=robot_x, idx7=robot_y, idx8=robot_yaw
    => call eeconn.sendposes
    """
    if len(msg.data)<9:
        rospy.logwarn("[my_ethoengine_connector] /osr_pose => data len<9 => skip")
        return
    owner_pose= [ msg.data[0]*1000, msg.data[1]*1000+1500, msg.data[2]*1000+200 ]
    stranger_pose= [ msg.data[3]*1000, msg.data[4]*1000+1500, msg.data[5]*1000+200 ]
    robot_pose= [ msg.data[6]*1000, msg.data[7]*1000+1500, msg.data[8] ]

    rospy.loginfo(f"[my_ethoengine_connector] => O={owner_pose}, S={stranger_pose}, M={robot_pose}")
    eeconn.sendposes(owner_pose, stranger_pose, robot_pose)

if __name__=="__main__":
    rospy.init_node("my_ethoengine_connector", anonymous=True)

    host= rospy.get_param("~ee_host","127.0.0.1")
    port= rospy.get_param("~ee_port",60000)

    eeconn= EthoEngineConnector(host, port)

    sub= rospy.Subscriber("/osr_pose", Float32MultiArray, osr_pose_callback, queue_size=1)

    rospy.loginfo("[my_ethoengine_connector] Subscribed /osr_pose => bounding & send to EthoEngine.")
    rospy.spin()

