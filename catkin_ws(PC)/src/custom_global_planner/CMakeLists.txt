cmake_minimum_required(VERSION 3.0.2)
project(custom_global_planner)

## 使用 C++11 编译
add_compile_options(-std=c++11)    # *** 新增 ***

## 查找依赖包
find_package(catkin REQUIRED COMPONENTS
  costmap_2d
  geometry_msgs
  nav_core
  nav_msgs
  pluginlib
  roscpp
  tf2_ros
)

###################################
## catkin 包配置 ##
###################################
catkin_package(
  INCLUDE_DIRS include                    # *** 新增 ***
  LIBRARIES custom_global_planner         # *** 新增 ***
  CATKIN_DEPENDS costmap_2d geometry_msgs nav_core nav_msgs pluginlib roscpp tf2_ros
)

###########
## 构建 ##
###########
include_directories(
  include
  ${catkin_INCLUDE_DIRS}
)

## 声明库
add_library(${PROJECT_NAME}
  src/custom_path_planner.cpp             # *** 新增 ***
)

## 由于未生成消息，所以不需要 add_dependencies
# add_dependencies(${PROJECT_NAME} ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

## 链接依赖库
target_link_libraries(${PROJECT_NAME}
  ${catkin_LIBRARIES}
)

#############
## 安装 ##
#############
install(TARGETS ${PROJECT_NAME}
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

install(DIRECTORY include/${PROJECT_NAME}/
  DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
)

install(FILES plugin_description.xml   # *** 新增：安装插件描述文件 ***
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
)

