#!/usr/bin/env python3
import rospy
from std_msgs.msg import String
from geometry_msgs.msg import PolygonStamped, Point32
from sensor_msgs.msg import PointCloud2, PointField
import sensor_msgs.point_cloud2 as pc2
import tf2_ros
import tf2_geometry_msgs
import math

class EdgeObstacleDualPublisher:
    def __init__(self):
        rospy.init_node('edge_obstacle_dual_publisher')

        # (A) 发布: PolygonStamped & PointCloud2
        self.poly_pub = rospy.Publisher('/edge_obstacle_polygon', PolygonStamped, queue_size=10)
        self.cloud_pub = rospy.Publisher('/edge_obstacle_cloud', PointCloud2, queue_size=10)

        # (B) 订阅: 哪个传感器检测到边缘(String)
        rospy.Subscriber('/edge_detected_sensor', String, self.edge_callback)

        self.tf_buffer = tf2_ros.Buffer()
        self.tf_listener = tf2_ros.TransformListener(self.tf_buffer)

        rospy.loginfo("EdgeObstacleDualPublisher node initialized.")

        # --- CHANGE START ---
        # 1) 设定机器人圆心到传感器的半径(7.5cm),
        #    并给每个传感器一个"安装角度" sensorAngle(相对于 base_link x 轴, 逆时针为正)
        #    *但是你反馈“左/右”跟之前相反，故此处交换:
        #       '/sensor1/distance' -> 右侧  => angle = -60 or? 这里实际配-120(详见下)
        #       '/sensor3/distance' -> 左侧  => angle = +120
        #       '/sensor2/distance' -> 中间 => 180(假设正前是 -x, 则前方=180°)
        #
        #   这里决定: base_link x 轴"正方向"是 0°, 逆时针增大.
        #   如果你的前方实际是 base_link "负 x" 轴, 那就是 angle=180 表示正前. 
        #   右侧 => angle=(180 -60)=120? 还是(180 +60)=240? 
        #   根据你需要"右"在 -60 还是 +60? 
        #
        #   这里给出示例:
        #     中间: angle=180°   (负x方向)
        #     右传感器: angle= -120° or 240°  (两者等价, 240=180+60)
        #     左传感器: angle= +120° or 60°?  (这里选+120=180-60?)
        #
        #   *你可根据现场观察再调整
        #
        self.radius = 0.075  # 7.5cm
        # angle dict: sensor -> (angle in deg)
        # 中间: 180°, 右: -120°, 左: +120°
        self.sensor_angles = {
            '/sensor2/distance': 180.0,   # 中间
            '/sensor1/distance': -120.0,  # 右
            '/sensor3/distance': 120.0    # 左
        }

        # 2) 矩形尺寸. "length" => 沿切线方向
        self.length = 0.1
        self.width  = 0.02
        # --- CHANGE END ---

    def edge_callback(self, msg):
        sensor_id = msg.data
        if sensor_id not in self.sensor_angles:
            rospy.logwarn(f"[DualPublisher] Unknown sensor ID: {sensor_id}")
            return
        rospy.loginfo(f"[DualPublisher] Edge detected from sensor: {sensor_id}")

        # 发布多边形 & 点云
        self.publish_polygon(sensor_id)
        self.publish_cloud(sensor_id)

    # --- CHANGE START ---
    # 3) 定义一个旋转函数, 将局部点(不做平移)绕(0,0)旋转 angleDeg
    def rotate_point(self, px, py, angleDeg):
        rad = math.radians(angleDeg)
        cosA = math.cos(rad)
        sinA = math.sin(rad)
        rx = px*cosA - py*sinA
        ry = px*sinA + py*cosA
        return (rx, ry)

    def publish_polygon(self, sensor_id):
        # (a) 取得该传感器相对于 base_link 的 "安装角度" sensorAngle
        sensorAngle = self.sensor_angles[sensor_id]

        # => 线"圆心->传感器"角度 = sensorAngle
        # => 切线方向 = sensorAngle + 90
        tangentAngle = sensorAngle + 90.0

        # (b) 在 "局部坐标" 下, 定义 x in [0, length], y in [-width/2, +width/2]
        #     这里(0,0)是传感器位置, "x方向"对应 切线, "y方向"对应 "切线左正"
        rect_local = [
            (0.0,          -self.width/2),
            (0.0,           self.width/2),
            ( self.length,  self.width/2),
            ( self.length, -self.width/2)
        ]

        # (c) 对 rect_local 逐点旋转 (tangentAngle), 得到 "base_link"下坐标(未平移)
        rect_rotated = []
        for (lx, ly) in rect_local:
            rx, ry = self.rotate_point(lx, ly, tangentAngle)
            rect_rotated.append((rx, ry))

        # (d) 传感器与圆心距离= self.radius
        #     传感器在 base_link 的实际中心坐标:
        sx = self.radius * math.cos(math.radians(sensorAngle))
        sy = self.radius * math.sin(math.radians(sensorAngle))

        # (e) 将旋转后的点平移 (sx, sy)
        rect_base = []
        for (rx, ry) in rect_rotated:
            rect_base.append( (rx + sx, ry + sy) )

        # (f) Transform到 map坐标
        polygon_msg = PolygonStamped()
        polygon_msg.header.frame_id = 'map'
        polygon_msg.header.stamp = rospy.Time.now()
        try:
            tf_stamped = self.tf_buffer.lookup_transform(
                'map', 'base_link', rospy.Time(0), rospy.Duration(1.0)
            )
        except (tf2_ros.LookupException, tf2_ros.ConnectivityException, tf2_ros.ExtrapolationException) as e:
            rospy.logwarn(f"[DualPublisher] TF failed(polygon): {e}")
            return

        for (bx, by) in rect_base:
            pt_base = tf2_geometry_msgs.PointStamped()
            pt_base.header.frame_id = 'base_link'
            pt_base.point.x = bx
            pt_base.point.y = by
            pt_base.point.z = 0.0

            pt_map = tf2_geometry_msgs.do_transform_point(pt_base, tf_stamped)
            polygon_msg.polygon.points.append(
                Point32(pt_map.point.x, pt_map.point.y, pt_map.point.z)
            )

        self.poly_pub.publish(polygon_msg)
        rospy.loginfo("[DualPublisher] Polygon published.")

    def publish_cloud(self, sensor_id):
        sensorAngle = self.sensor_angles[sensor_id]
        tangentAngle = sensorAngle + 90.0

        # 先定义局部坐标点( x in [0, length], y in [-width/2, +width/2] )
        dx=0.005; dy=0.005
        half_w = self.width/2

        local_points = []
        y_ = -half_w
        while y_ <= half_w:
            x_ = 0.0
            while x_ <= self.length:
                local_points.append((x_, y_, 0.0))
                x_ += dx
            y_ += dy

        # 先做"旋转",再做"平移"
        rotated_points = []
        for (lx, ly, lz) in local_points:
            rx, ry = self.rotate_point(lx, ly, tangentAngle)
            rotated_points.append((rx, ry, 0.0))

        # 传感器实际在 base_link 下坐标
        sx = self.radius * math.cos(math.radians(sensorAngle))
        sy = self.radius * math.sin(math.radians(sensorAngle))

        # 平移
        points_base = []
        for (rx, ry, rz) in rotated_points:
            points_base.append((rx + sx, ry + sy, rz))

        # transform到 map
        try:
            tf_stamped = self.tf_buffer.lookup_transform(
                'map','base_link', rospy.Time(0), rospy.Duration(1.0)
            )
        except (tf2_ros.LookupException, tf2_ros.ConnectivityException, tf2_ros.ExtrapolationException) as e:
            rospy.logwarn(f"[DualPublisher] TF failed(cloud): {e}")
            return

        transformed_points = []
        for (bx, by, bz) in points_base:
            pt_base = tf2_geometry_msgs.PointStamped()
            pt_base.header.frame_id = 'base_link'
            pt_base.point.x = bx
            pt_base.point.y = by
            pt_base.point.z = bz

            pt_map = tf2_geometry_msgs.do_transform_point(pt_base, tf_stamped)
            transformed_points.append(
                (pt_map.point.x, pt_map.point.y, pt_map.point.z)
            )

        cloud_msg = self.create_pointcloud2(transformed_points, 'map')
        self.cloud_pub.publish(cloud_msg)
        rospy.loginfo(f"[DualPublisher] Cloud published. {len(transformed_points)} points")
    # --- CHANGE END ---

    def create_pointcloud2(self, points, frame_id='map'):
        header = rospy.Header()
        header.stamp = rospy.Time.now()
        header.frame_id = frame_id

        fields = [
            PointField('x', 0,  PointField.FLOAT32, 1),
            PointField('y', 4,  PointField.FLOAT32, 1),
            PointField('z', 8,  PointField.FLOAT32, 1),
        ]
        return pc2.create_cloud(header, fields, points)

    def spin(self):
        rospy.spin()

if __name__ == '__main__':
    node = EdgeObstacleDualPublisher()
    node.spin()

