#!/usr/bin/env python3
import socket
import rospy
from geometry_msgs.msg import PoseStamped, Point, Quaternion
from nav_msgs.msg import Path  # *** 新增：发布路径 ***

ENGINE_HOST = "127.0.0.1"
ENGINE_PORT = 45678
FIELD_CNT   = 15
Y_OFFSET    = 1500.0

rospy.init_node("ethoengine_output_node")
goal_pub = rospy.Publisher("/move_base_simple/goal", PoseStamped, queue_size=1)
path_pub = rospy.Publisher("/custom_global_path", Path, queue_size=1)  # *** 新增 ***

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
rospy.loginfo(f"Connecting EthoEngine {ENGINE_HOST}:{ENGINE_PORT}")
sock.connect((ENGINE_HOST, ENGINE_PORT))
rospy.loginfo("Connected.")

pose_msg = PoseStamped()
pose_msg.header.frame_id = "map"
pose_msg.pose.orientation = Quaternion(0, 0, 0, 1)

buffer = b""
rate   = rospy.Rate(10)   # 按10Hz读取

def extract_one_message(buf: bytes):
    try:
        s = buf.decode("ascii", errors="ignore")
    except UnicodeDecodeError:
        return None, buf
    if '\n' not in s:
        return None, buf
    full, remain = s.split('\n', 1)
    return full, remain.encode("ascii")

try:
    while not rospy.is_shutdown():
        chunk = sock.recv(1024)
        if not chunk:
            raise ConnectionError("socket closed")
        buffer += chunk

        msg_str, buffer = extract_one_message(buffer)
        if not msg_str:
            continue

        # *** 修改：按分号分割出 15 字段和路径部分 ***
        parts = msg_str.split(';')
        header = parts[0]  # 15字段
        path_parts = parts[1:] if len(parts) > 1 else []

        fields = header.split(',')
        if len(fields) < FIELD_CNT:
            rospy.logwarn("bad header: " + header)
            continue
        try:
            tgx = float(fields[13])
            tgy = float(fields[14])
        except ValueError:
            rospy.logwarn("bad data: " + header)
            continue

        # 解析 tmp_goal 生成目标点
        global_x = tgx / 1000.0
        global_y = (tgy - Y_OFFSET) / 1000.0
        pose_msg.header.stamp = rospy.Time.now()
        pose_msg.pose.position = Point(global_x, global_y, 0.0)
        goal_pub.publish(pose_msg)

        # *** 新增：解析路径并发布 Path ***
        path_msg = Path()
        path_msg.header = pose_msg.header
        path_msg.poses = []
        for seg in path_parts:
            if not seg:
                continue
            try:
                x_str, y_str = seg.split(',', 1)
                px = float(x_str) / 1000.0
                py = (float(y_str) - Y_OFFSET) / 1000.0
            except ValueError:
                continue
            p = PoseStamped()
            p.header = pose_msg.header
            p.pose.position.x = px
            p.pose.position.y = py
            p.pose.position.z = 0.0
            p.pose.orientation.w = 1.0
            path_msg.poses.append(p)
        # 发布全局路径
        path_pub.publish(path_msg)

        rate.sleep()

except (socket.error, ConnectionError) as e:
    rospy.logerr("Socket error: %s", e)
finally:
    sock.close()

