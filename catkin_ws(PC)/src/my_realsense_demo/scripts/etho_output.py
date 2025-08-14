#!/usr/bin/env python3
import socket, rospy
from geometry_msgs.msg import PoseStamped, Point, Quaternion

ENGINE_HOST, ENGINE_PORT = "127.0.0.1", 45678
Y_OFFSET   = 1500.0      # mm
FIELD_CNT  = 15          # 一共 15 个数字

rospy.init_node("ethoengine_output_node")
pub = rospy.Publisher("/move_base_simple/goal", PoseStamped, queue_size=1)

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
rospy.loginfo(f"Connecting EthoEngine {ENGINE_HOST}:{ENGINE_PORT}")
sock.connect((ENGINE_HOST, ENGINE_PORT))
rospy.loginfo("Connected.")

pose_msg = PoseStamped()
pose_msg.header.frame_id = "map"
pose_msg.pose.orientation = Quaternion(0, 0, 0, 1)

buffer = b""
rate   = rospy.Rate(10)   # 1 Hz 发布

def extract_one_message(buf: bytes):
    """按 '\\n' 拆帧，返回 (一帧字符串, 剩余字节)"""
    if b'\n' not in buf:
        return None, buf
    line, remain = buf.split(b'\n', 1)
    return line.decode('ascii', errors='ignore').strip(), remain

try:
    while not rospy.is_shutdown():
        chunk = sock.recv(256)
        if not chunk:
            raise ConnectionError("socket closed")
        buffer += chunk

        msg_str, buffer = extract_one_message(buffer)
        if not msg_str:
            continue

        fields = msg_str.split(",")
        if len(fields) != FIELD_CNT:
            rospy.logwarn("Bad frame (field num): " + msg_str)
            continue
        try:
            tgx = float(fields[13])        # mm
            tgy = float(fields[14])        # mm
        except ValueError:
            rospy.logwarn("Bad float: " + msg_str)
            continue

        # mm → m，并把 y 轴平移回原坐标系
        global_x = tgx / 1000.0
        global_y = (tgy - Y_OFFSET) / 1000.0

        pose_msg.header.stamp = rospy.Time.now()
        pose_msg.pose.position = Point(global_x, global_y, 0.0)
        pub.publish(pose_msg)
        rospy.loginfo(f"Goal -> x:{global_x:.3f} m  y:{global_y:.3f} m")

        rate.sleep()

except (socket.error, ConnectionError) as e:
    rospy.logerr("Socket error: %s", e)
finally:
    sock.close()

