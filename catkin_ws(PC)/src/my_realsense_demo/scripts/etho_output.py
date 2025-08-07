#!/usr/bin/env python3
import socket
import rospy
from geometry_msgs.msg import PoseStamped, Point, Quaternion

ENGINE_HOST = "127.0.0.1"
ENGINE_PORT = 45678
Y_OFFSET    = 1500.0          # 输入时加过 1500，这里要减回去
FIELD_CNT   = 15              # *** MOD *** 发送端总共有 15 个字段

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
rate   = rospy.Rate(1)        # *** MOD *** 5 Hz 发布

def extract_one_message(buf: bytes):
    """从累计缓冲里取出一个完整逗号串（15 个字段）"""
    try:
        s = buf.decode("ascii", errors="ignore")
    except UnicodeDecodeError:
        return None, b""

    # 至少出现 14 个逗号才可能是完整报文
    if s.count(",") < FIELD_CNT - 1:
        return None, buf  # 继续累计

    # 取到第 15 个字段为止
    parts = s.split(",")
    full  = ",".join(parts[:FIELD_CNT])
    remain = ",".join(parts[FIELD_CNT:]).encode("ascii", errors="ignore")
    return full, remain

try:
    while not rospy.is_shutdown():
        # 读取网络数据（非阻塞大循环）
        chunk = sock.recv(256)
        if not chunk:
            raise ConnectionError("socket closed")
        buffer += chunk

        msg_str, buffer = extract_one_message(buffer)
        if not msg_str:
            continue   # 报文未凑齐

        fields = msg_str.split(",")
        try:
            # *** MOD *** 14、15 两项就是 tmp_goal
            tgx = float(fields[13])
            tgy = float(fields[14])
        except (ValueError, IndexError):
            rospy.logwarn("bad data: " + msg_str)
            continue

        # 坐标系转换
        global_x = tgx / 1000
        global_y = (tgy - Y_OFFSET) / 1000

        # 发布
        pose_msg.header.stamp = rospy.Time.now()
        pose_msg.pose.position = Point(global_x, global_y, 0.0)
        pub.publish(pose_msg)
        rospy.loginfo(f"Goal -> x:{global_x:.1f}  y:{global_y:.1f}")

        rate.sleep()



except (socket.error, ConnectionError) as e:
    rospy.logerr("Socket error: %s", e)
finally:
    sock.close()

