#!/usr/bin/env bash
#
# create_my_realsense_demo.sh
# 
# 说明：
#   1) 在 ~/catkin_ws/src/ 下创建 my_realsense_demo 包
#   2) 写入简单的 package.xml, CMakeLists.txt, launch/..., scripts/...
#   3) 适合使用 catkin build 或 catkin_make 的工作空间
#

# 1) 切换到你的 catkin_ws/src
cd ~/catkin_ws/src

# 2) 创建包文件夹
mkdir -p my_realsense_demo
cd my_realsense_demo

# 3) 创建 launch/ 和 scripts/ 目录
mkdir -p launch
mkdir -p scripts

# 4) 生成最简 package.xml
cat <<EOF > package.xml
<package format="2">
  <name>my_realsense_demo</name>
  <version>0.0.1</version>
  <description>Demo for realsense image republish & subscribe</description>

  <maintainer email="you@example.com">Your Name</maintainer>
  <license>BSD</license>

  <buildtool_depend>catkin</buildtool_depend>

  <build_depend>image_transport</build_depend>
  <build_depend>cv_bridge</build_depend>
  <build_depend>roscpp</build_depend>
  <build_depend>rospy</build_depend>

  <exec_depend>image_transport</exec_depend>
  <exec_depend>cv_bridge</exec_depend>
  <exec_depend>roscpp</exec_depend>
  <exec_depend>rospy</exec_depend>
</package>
EOF

# 5) 生成最简 CMakeLists.txt
cat <<EOF > CMakeLists.txt
cmake_minimum_required(VERSION 3.0.2)
project(my_realsense_demo)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
  sensor_msgs
  cv_bridge
  image_transport
)

catkin_package()

include_directories(\${catkin_INCLUDE_DIRS})

# 如果只是纯Python节点，无需 add_executable
# 用 catkin_install_python 安装脚本
catkin_install_python(
  PROGRAMS scripts/face_recognizer_ros.py
  DESTINATION \${CATKIN_PACKAGE_BIN_DESTINATION}
)
EOF

# 6) launch/republish_and_subscribe.launch
cat <<EOF > launch/republish_and_subscribe.launch
<launch>
  <!-- Republish color (compressed -> raw) -->
  <node pkg="image_transport" type="republish" name="color_repub"
        args="compressed in:=/camera/color/image_raw raw out:=/camera/color/image_raw_uncompressed"/>

  <!-- Republish depth (compressedDepth -> raw) -->
  <node pkg="image_transport" type="republish" name="depth_repub"
        args="compressedDepth in:=/camera/depth/image_rect_raw raw out:=/camera/depth/image_rect_raw_uncompressed"/>

  <!-- Python node to subscribe uncompressed topics -->
  <node pkg="my_realsense_demo" type="face_recognizer_ros.py" name="face_recognizer_node" output="screen"/>
</launch>
EOF

# 7) scripts/face_recognizer_ros.py
cat <<EOF > scripts/face_recognizer_ros.py
#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import rospy
import cv2
import numpy as np
from sensor_msgs.msg import Image
from cv_bridge import CvBridge

class FaceRecognizerROS:
    def __init__(self):
        rospy.loginfo("Initializing FaceRecognizerROS node...")
        self.bridge = CvBridge()

        # Subscribe uncompressed color
        self.sub_color = rospy.Subscriber(
            "/camera/color/image_raw_uncompressed",
            Image,
            self.color_cb, queue_size=1
        )
        # Subscribe uncompressed depth
        self.sub_depth = rospy.Subscriber(
            "/camera/depth/image_rect_raw_uncompressed",
            Image,
            self.depth_cb, queue_size=1
        )
        rospy.loginfo("Subscription set up. Waiting for images...")

    def color_cb(self, msg):
        color_img = self.bridge.imgmsg_to_cv2(msg, "bgr8")
        # Display
        cv2.imshow("Color", color_img)
        cv2.waitKey(1)

    def depth_cb(self, msg):
        # Possibly 16UC1 or 32FC1
        depth_img = self.bridge.imgmsg_to_cv2(msg, "passthrough")
        # Not displayed by default. You can do further processing here.

def main():
    rospy.init_node("face_recognizer_ros", anonymous=True)
    node = FaceRecognizerROS()
    rospy.spin()
    cv2.destroyAllWindows()

if __name__=="__main__":
    main()
EOF

chmod +x scripts/face_recognizer_ros.py

echo "my_realsense_demo package structure created successfully!"
