#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import socket
import math
import rospy
from geometry_msgs.msg import PoseStamped, Point, Quaternion
from nav_msgs.msg import Path

# ---------------- 参数 ----------------
ENGINE_HOST   = rospy.get_param("~host", "127.0.0.1")
ENGINE_PORT   = int(rospy.get_param("~port", 45678))
FIELD_CNT     = int(rospy.get_param("~field_cnt", 15))
Y_OFFSET_MM   = float(rospy.get_param("~y_offset_mm", 1500.0))

GOAL_EPS_M    = float(rospy.get_param("~goal_eps_xy", 0.20))     # 目标变化阈值(m)
END_EPS_M     = float(rospy.get_param("~endpoint_eps", 0.20))    # 末端变化阈值(m)
MIN_PERIOD_S  = float(rospy.get_param("~min_period", 0.5))       # 最小发布间隔(s)
REPUB_PERIOD_S= float(rospy.get_param("~republish_period", 3.0)) # ★修改：保活重发(s)
SIG_ROUND     = int(rospy.get_param("~path_round", 2))
LATCH_PATH    = bool(rospy.get_param("~path_latch", True))       # ★修改：路径采用latch

rospy.init_node("ethoengine_output_node", anonymous=False)

goal_pub = rospy.Publisher("/move_base_simple/goal", PoseStamped, queue_size=1)
path_pub = rospy.Publisher("/custom_global_path", Path, queue_size=1, latch=LATCH_PATH)  # ★修改：latch

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.settimeout(0.05)  # ★修改：短超时，避免阻塞
rospy.loginfo("Connecting EthoEngine %s:%d", ENGINE_HOST, ENGINE_PORT)
sock.connect((ENGINE_HOST, ENGINE_PORT))
rospy.loginfo("Connected.")

pose_msg = PoseStamped()
pose_msg.header.frame_id = "map"
pose_msg.pose.orientation = Quaternion(0, 0, 0, 1)

buffer = b""

last_goal_xy   = None
last_goal_time = rospy.Time(0)
last_path_sig  = None
last_path_end  = None
last_path_time = rospy.Time(0)

def extract_one_message(buf: bytes):
    try:
        s = buf.decode("ascii", errors="ignore")
    except UnicodeDecodeError:
        return None, buf
    i = s.find('\n')
    if i < 0:
        return None, buf
    return s[:i], s[i+1:].encode("ascii")

def mm2m(v): return v / 1000.0
def dist(ax, ay, bx, by): return math.hypot(ax - bx, ay - by)

def path_signature(points, rnd=2):
    if not points:
        return (0, 0, 0, 0, 0)
    fx, fy = points[0]
    lx, ly = points[-1]
    return (len(points), round(fx, rnd), round(fy, rnd), round(lx, rnd), round(ly, rnd))

rate = rospy.Rate(200)

try:
    while not rospy.is_shutdown():
        # 非阻塞读socket
        try:
            chunk = sock.recv(4096)
            if not chunk:
                raise ConnectionError("socket closed")
            buffer += chunk
        except socket.timeout:
            rate.sleep()
            continue

        # 逐行处理
        while True:
            line, buffer = extract_one_message(buffer)
            if not line:
                break

            parts  = line.strip().split(';')
            header = parts[0]
            fields = header.split(',')
            if len(fields) < FIELD_CNT:
                rospy.logwarn_throttle(2.0, "bad header: %s", header)
                continue

            # tmp_goal(mm)
            try:
                tgx_mm = float(fields[13])
                tgy_mm = float(fields[14])
            except ValueError:
                rospy.logwarn_throttle(2.0, "bad data: %s", header)
                continue

            gx = mm2m(tgx_mm)
            gy = mm2m(tgy_mm - Y_OFFSET_MM)
            now = rospy.Time.now()

            # -------- 目标：去抖 + 保活 --------
            need_goal = False
            if last_goal_xy is None or dist(gx, gy, *last_goal_xy) > GOAL_EPS_M:
                need_goal = True
            if (now - last_goal_time).to_sec() < MIN_PERIOD_S:
                need_goal = False
            if (now - last_goal_time).to_sec() >= REPUB_PERIOD_S:  # ★修改：保活重发
                need_goal = True

            if need_goal:
                pose_msg.header.stamp = now
                pose_msg.pose.position = Point(gx, gy, 0.0)
                goal_pub.publish(pose_msg)
                last_goal_xy = (gx, gy)
                last_goal_time = now
                rospy.logdebug("Publish GOAL x=%.3f y=%.3f", gx, gy)

            # -------- 路径：解析 + 去抖 + 保活 --------
            pts = []
            for seg in parts[1:]:
                if not seg:
                    continue
                try:
                    xs, ys = seg.split(',', 1)
                    px = mm2m(float(xs))
                    py = mm2m(float(ys) - Y_OFFSET_MM)
                    pts.append((px, py))
                except Exception:
                    continue

            sig = path_signature(pts, SIG_ROUND)
            end_xy = pts[-1] if pts else None

            need_path = False
            if sig != last_path_sig:
                need_path = True
            if end_xy and last_path_end and dist(end_xy[0], end_xy[1], last_path_end[0], last_path_end[1]) > END_EPS_M:
                need_path = True
            if (now - last_path_time).to_sec() < MIN_PERIOD_S:
                need_path = False
            if (now - last_path_time).to_sec() >= REPUB_PERIOD_S:  # ★修改：保活重发
                need_path = True

            if need_path:
                path_msg = Path()
                path_msg.header.stamp = now
                path_msg.header.frame_id = "map"
                for (px, py) in pts:
                    ps = PoseStamped()
                    ps.header = path_msg.header
                    ps.pose.position.x = px
                    ps.pose.position.y = py
                    ps.pose.orientation.w = 1.0
                    path_msg.poses.append(ps)
                path_pub.publish(path_msg)
                last_path_sig  = sig
                last_path_end  = end_xy
                last_path_time = now
                rospy.logdebug("Publish PATH len=%d sig=%s", len(pts), str(sig))

        rate.sleep()

except (socket.error, ConnectionError) as e:
    rospy.logerr("Socket error: %s", e)
finally:
    try:
        sock.close()
    except Exception:
        pass

