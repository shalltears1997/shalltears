# generated from catkin/cmake/template/pkg.context.pc.in
CATKIN_PACKAGE_PREFIX = ""
PROJECT_PKG_CONFIG_INCLUDE_DIRS = "${prefix}/include".split(';') if "${prefix}/include" != "" else []
PROJECT_CATKIN_DEPENDS = "costmap_2d;geometry_msgs;nav_core;nav_msgs;pluginlib;roscpp;tf2_ros".replace(';', ' ')
PKG_CONFIG_LIBRARIES_WITH_PREFIX = "-lcustom_global_planner".split(';') if "-lcustom_global_planner" != "" else []
PROJECT_NAME = "custom_global_planner"
PROJECT_SPACE_DIR = "/home/niitsuma/catkin_ws/install"
PROJECT_VERSION = "0.0.1"
