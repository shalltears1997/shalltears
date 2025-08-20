#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import rospy
from geometry_msgs.msg import Twist

class CmdVelSmoother:
    def __init__(self):
        # ---- 参数（可在 launch 里覆盖）----
        self.in_topic   = rospy.get_param("~in_topic",  "/nav_cmd_vel")
        self.out_topic  = rospy.get_param("~out_topic", "/rover_drive")
        self.rate_hz    = float(rospy.get_param("~rate", 30.0))   # 平滑发布频率
        # 线速度(m/s)与角速度(rad/s)的最大加/减速度
        self.ax_up      = float(rospy.get_param("~accel_x",   0.30))
        self.ax_down    = float(rospy.get_param("~decel_x",   0.40))
        self.aw_up      = float(rospy.get_param("~accel_w",   1.20))
        self.aw_down    = float(rospy.get_param("~decel_w",   1.50))
        # 目标速度的一阶低通时间常数（秒）；设 0 关闭低通
        self.tau        = float(rospy.get_param("~target_tau", 0.20))
        # 若暂时收不到新指令，保持上一个目标不变的时间（秒）
        self.hold_sec   = float(rospy.get_param("~hold_time",  0.30))
        # 线速/角速输出上限（防御性限制，可不设）
        self.vx_max     = float(rospy.get_param("~vx_max",  0.25))
        self.wz_max     = float(rospy.get_param("~wz_max",  1.00))

        self.target_vx      = 0.0   # 上游最新目标（经低通后）
        self.target_wz      = 0.0
        self.raw_target_vx  = 0.0   # 上游原始目标（未低通）
        self.raw_target_wz  = 0.0
        self.have_target    = False
        self.last_target_ts = rospy.Time(0)

        self.cur_vx = 0.0          # 当前输出状态
        self.cur_wz = 0.0

        self.sub = rospy.Subscriber(self.in_topic, Twist, self.cb, queue_size=1)
        self.pub = rospy.Publisher(self.out_topic, Twist, queue_size=1)

        self.dt = 1.0 / self.rate_hz
        self.timer = rospy.Timer(rospy.Duration.from_sec(self.dt), self.on_timer)

    def cb(self, msg: Twist):
        self.raw_target_vx = self._clip(msg.linear.x,  -self.vx_max, self.vx_max)
        self.raw_target_wz = self._clip(msg.angular.z, -self.wz_max, self.wz_max)
        self.last_target_ts = rospy.Time.now()
        self.have_target = True

    @staticmethod
    def _clip(x, lo, hi):
        return max(lo, min(hi, x))

    def _lowpass(self, prev, target, dt, tau):
        if tau <= 0.0:
            return target
        alpha = dt / (tau + dt)
        return (1.0 - alpha) * prev + alpha * target

    def _ramp(self, cur, tgt, dt, a_up, a_dn):
        # 以加/减速度限制把 cur 往 tgt 逼近
        if tgt > cur:
            cur += min(a_up*dt, tgt - cur)
        else:
            cur -= min(a_dn*dt, cur - tgt)
        return cur

    def on_timer(self, _evt):
        now = rospy.Time.now()

        # 1) 生成“目标”（先 hold，再低通）
        if self.have_target and (now - self.last_target_ts).to_sec() <= self.hold_sec:
            # 保持最近目标；对目标做轻微低通
            tgt_vx = self._lowpass(self.target_vx, self.raw_target_vx, self.dt, self.tau)
            tgt_wz = self._lowpass(self.target_wz, self.raw_target_wz, self.dt, self.tau)
        else:
            # 超过 hold_time 没有新指令：目标回 0（软刹车）
            tgt_vx = self._lowpass(self.target_vx, 0.0, self.dt, self.tau)
            tgt_wz = self._lowpass(self.target_wz, 0.0, self.dt, self.tau)

        self.target_vx, self.target_wz = tgt_vx, tgt_wz

        # 2) 以加/减速度限幅从当前状态跟随目标（斜坡）
        self.cur_vx = self._ramp(self.cur_vx, self.target_vx, self.dt, self.ax_up, self.ax_down)
        self.cur_wz = self._ramp(self.cur_wz, self.target_wz, self.dt, self.aw_up, self.aw_down)

        # 3) 发布
        out = Twist()
        out.linear.x  = self.cur_vx
        out.angular.z = self.cur_wz
        self.pub.publish(out)

if __name__ == "__main__":
    rospy.init_node("cmd_vel_smoother")
    CmdVelSmoother()
    rospy.spin()
