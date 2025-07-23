#!/usr/bin/env python3
import rospy
from std_msgs.msg import String
from geometry_msgs.msg import PolygonStamped, Point32
from nav_msgs.msg import Odometry
import tf
import tf2_ros
import tf2_geometry_msgs

class ObstaclePublisher:
    def __init__(self):
        rospy.init_node('obstacle_publisher')
        self.obstacle_pub = rospy.Publisher('/edge_obstacle', PolygonStamped, queue_size=10)
        rospy.Subscriber('/edge_detected', String, self.edge_callback)
        self.tf_buffer = tf2_ros.Buffer()
        self.tf_listener = tf2_ros.TransformListener(self.tf_buffer)
        self.edge_detected = False

        # 定义传感器在机器人坐标系中的位置（单位：米）
        self.sensor_positions = {
            '/sensor1/distance': {'x': 0.05, 'y': 0.05},  # 左前方传感器
            '/sensor2/distance': {'x': 0.1, 'y': 0.0},    # 中间传感器
            '/sensor3/distance': {'x': 0.05, 'y': -0.05}  # 右前方传感器
        }

    def edge_callback(self, msg):
        sensor_id = msg.data
        rospy.loginfo(f"Publishing obstacle for sensor {sensor_id}")
        self.publish_obstacle(sensor_id)

    def publish_obstacle(self, sensor_id):
        try:
            # 获取传感器在机器人坐标系中的位置
            sensor_pos = self.sensor_positions.get(sensor_id, None)
            if sensor_pos is None:
                rospy.logwarn(f"Unknown sensor ID: {sensor_id}")
                return

            # 创建障碍物的中心点（在 base_link 坐标系中）
            obstacle_center = Point32()
            obstacle_center.x = sensor_pos['x']
            obstacle_center.y = sensor_pos['y']
            obstacle_center.z = 0.0

            # 将中心点转换到地图坐标系
            point_stamped = tf2_geometry_msgs.PointStamped()
            point_stamped.header.frame_id = 'base_link'
            point_stamped.header.stamp = rospy.Time.now()
            point_stamped.point.x = obstacle_center.x
            point_stamped.point.y = obstacle_center.y
            point_stamped.point.z = obstacle_center.z

            transform = self.tf_buffer.lookup_transform('map', 'base_link', rospy.Time(0), rospy.Duration(1.0))
            obstacle_center_map = tf2_geometry_msgs.do_transform_point(point_stamped, transform)

            # 定义障碍物的形状（矩形），尺寸为长 0.1 米、宽 0.02 米
            length = 0.1  # 0.1 米 = 10 厘米
            width = 0.02  # 0.02 米 = 2 厘米

            # 计算障碍物的四个顶点（在地图坐标系中）
            x = obstacle_center_map.point.x
            y = obstacle_center_map.point.y

            p1 = Point32(x - length/2, y - width/2, 0.0)
            p2 = Point32(x - length/2, y + width/2, 0.0)
            p3 = Point32(x + length/2, y + width/2, 0.0)
            p4 = Point32(x + length/2, y - width/2, 0.0)

            # 创建 PolygonStamped 消息
            obstacle = PolygonStamped()
            obstacle.header.frame_id = "map"
            obstacle.header.stamp = rospy.Time.now()
            obstacle.polygon.points = [p1, p2, p3, p4]

            # 发布障碍物
            self.obstacle_pub.publish(obstacle)
            rospy.loginfo("Obstacle published.")
        except (tf2_ros.LookupException, tf2_ros.ConnectivityException, tf2_ros.ExtrapolationException) as e:
            rospy.logwarn(f"TF lookup failed: {e}")

    def spin(self):
        rospy.spin()

if __name__ == '__main__':
    try:
        publisher = ObstaclePublisher()
        publisher.spin()
    except rospy.ROSInterruptException:
        pass
