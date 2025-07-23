#!/usr/bin/env python3
import rospy
from std_msgs.msg import String
from geometry_msgs.msg import PolygonStamped, Point32, PointStamped
from sensor_msgs.msg import PointCloud2, PointField
import sensor_msgs.point_cloud2 as pc2

import tf2_ros
import tf2_geometry_msgs
import math

class EdgeObstacleDualPublisher:
    def __init__(self):
        rospy.init_node('edge_obstacle_dual_publisher')

        # (A) 发布Polygon到 RViz 显示
        self.poly_pub = rospy.Publisher('/edge_obstacle_polygon', PolygonStamped, queue_size=10)

        # (B) 发布PointCloud2到 costmap 识别
        self.cloud_pub = rospy.Publisher('/edge_obstacle_cloud', PointCloud2, queue_size=10)

        # 订阅 "哪一个传感器检测到边缘" 事件
        rospy.Subscriber('/edge_detected_sensor', String, self.edge_callback)

        # TF
        self.tf_buffer = tf2_ros.Buffer()
        self.tf_listener = tf2_ros.TransformListener(self.tf_buffer)

        # 定义3个传感器 在 base_link 下的位置、角度
        self.sensor_positions = {
            '/sensor1/distance': {'x': 0.05, 'y':  0.05, 'theta_deg':  60.0},
            '/sensor2/distance': {'x': 0.10, 'y':  0.00, 'theta_deg':   0.0},
            '/sensor3/distance': {'x': 0.05, 'y': -0.05, 'theta_deg': -60.0}
        }

        rospy.loginfo("EdgeObstacleDualPublisher node initialized.")

    def edge_callback(self, msg):
        sensor_id = msg.data
        rospy.loginfo(f"[DualPublisher] Edge detected from sensor: {sensor_id}")

        if sensor_id not in self.sensor_positions:
            rospy.logwarn(f"Unknown sensor ID: {sensor_id}")
            return

        # 同时发布 Polygon 和 PointCloud2
        self.publish_polygon(sensor_id)
        self.publish_cloud(sensor_id)

    # ================ 发布多边形 ==================
    def publish_polygon(self, sensor_id):
        """在 base_link 下生成一个旋转+平移后的矩形顶点, transform到 map, 发布 PolygonStamped."""
        sensor_info = self.sensor_positions[sensor_id]
        sx, sy = sensor_info['x'], sensor_info['y']
        theta_rad = math.radians(sensor_info['theta_deg'])

        # 1) 构造矩形 (中心在(0,0)), size: 0.1 x 0.02
        length = 0.1
        width  = 0.02
        rect_points = [
            (-length/2, -width/2),
            (-length/2,  width/2),
            ( length/2,  width/2),
            ( length/2, -width/2)
        ]

        def rotate_point(px, py, theta):
            cosA = math.cos(theta)
            sinA = math.sin(theta)
            rx = px*cosA - py*sinA
            ry = px*sinA + py*cosA
            return rx, ry

        # 2) 对矩形做旋转 & 平移(到传感器中心)
        final_points_base = []
        for (px, py) in rect_points:
            rx, ry = rotate_point(px, py, theta_rad)
            rx += sx
            ry += sy
            final_points_base.append((rx, ry))

        # 3) 逐点 transform 到 map
        polygon_msg = PolygonStamped()
        polygon_msg.header.frame_id = 'map'
        polygon_msg.header.stamp = rospy.Time.now()

        try:
            tf_stamped = self.tf_buffer.lookup_transform('map', 'base_link', rospy.Time(0), rospy.Duration(1.0))
        except (tf2_ros.LookupException, tf2_ros.ConnectivityException, tf2_ros.ExtrapolationException) as e:
            rospy.logwarn(f"publish_polygon TF error: {e}")
            return

        for (bx, by) in final_points_base:
            pt_base = tf2_geometry_msgs.PointStamped()
            pt_base.header.frame_id = 'base_link'
            pt_base.point.x = bx
            pt_base.point.y = by
            pt_base.point.z = 0.0
            pt_map = tf2_geometry_msgs.do_transform_point(pt_base, tf_stamped)
            polygon_msg.polygon.points.append(Point32(pt_map.point.x, pt_map.point.y, pt_map.point.z))

        # 4) 发布
        self.poly_pub.publish(polygon_msg)
        rospy.loginfo("[DualPublisher] Polygon published.")

    # ================ 发布点云 ==================
    def publish_cloud(self, sensor_id):
        """
        生成一片小矩形的离散点(0.1x0.02)，旋转+平移后，再 TF 到 map，并发布 PointCloud2
        """
        sensor_info = self.sensor_positions[sensor_id]
        sx, sy = sensor_info['x'], sensor_info['y']
        theta_rad = math.radians(sensor_info['theta_deg'])

        length = 0.1
        width  = 0.02
        dx = 0.005  # 采样间隔
        dy = 0.005

        def rotate_point(px, py, theta):
            cosA = math.cos(theta)
            sinA = math.sin(theta)
            rx = px*cosA - py*sinA
            ry = px*sinA + py*cosA
            return rx, ry

        points_base = []
        half_l = length/2
        half_w = width/2

        y_ = -half_w
        while y_ <= half_w:
            x_ = -half_l
            while x_ <= half_l:
                rx, ry = rotate_point(x_, y_, theta_rad)
                rx += sx
                ry += sy
                points_base.append((rx, ry, 0.0))
                x_ += dx
            y_ += dy

        # TF transform to map
        try:
            tf_stamped = self.tf_buffer.lookup_transform('map', 'base_link', rospy.Time(0), rospy.Duration(1.0))
        except (tf2_ros.LookupException, tf2_ros.ConnectivityException, tf2_ros.ExtrapolationException) as e:
            rospy.logwarn(f"publish_cloud TF error: {e}")
            return

        transformed_points = []
        for (bx, by, bz) in points_base:
            pt_base = tf2_geometry_msgs.PointStamped()
            pt_base.header.frame_id = 'base_link'
            pt_base.point.x = bx
            pt_base.point.y = by
            pt_base.point.z = bz

            pt_map = tf2_geometry_msgs.do_transform_point(pt_base, tf_stamped)
            transformed_points.append((pt_map.point.x, pt_map.point.y, pt_map.point.z))

        # 构造 PointCloud2
        pc2_msg = self.create_pointcloud2(transformed_points, 'map')

        # 发布
        self.cloud_pub.publish(pc2_msg)
        rospy.loginfo(f"[DualPublisher] Cloud published. {len(transformed_points)} points")

    def create_pointcloud2(self, points, frame_id='map'):
        """把 (x,y,z)列表封装成 PointCloud2."""
        header = rospy.Header()
        header.stamp = rospy.Time.now()
        header.frame_id = frame_id

        fields = [
            PointField('x', 0,  PointField.FLOAT32, 1),
            PointField('y', 4,  PointField.FLOAT32, 1),
            PointField('z', 8,  PointField.FLOAT32, 1)
        ]

        pc2_msg = pc2.create_cloud(header, fields, points)
        return pc2_msg

    def spin(self):
        rospy.spin()

if __name__ == '__main__':
    try:
        node = EdgeObstacleDualPublisher()
        node.spin()
    except rospy.ROSInterruptException:
        pass

