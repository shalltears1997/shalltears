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

include_directories(${catkin_INCLUDE_DIRS})

# 如果只是纯Python节点，无需 add_executable
# 用 catkin_install_python 安装脚本
catkin_install_python(

  PROGRAMS scripts/face_recognizer_ros.py
           scripts/face_recognizer_republish.py
           scripts/face_recognizer_no_yunet.py
           scripts/yunet_face_recognizer_test_depth.py
  DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)
