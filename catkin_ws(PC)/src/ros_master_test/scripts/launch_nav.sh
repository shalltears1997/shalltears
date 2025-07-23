#!/usr/bin/env bash
################################################################################
# launch_gmapping.sh
#
# 功能：
#   1) 树莓派: roslaunch lightrover_ros gmapping.launch
#   2) 等 5 秒
#   3) 本地 PC: roslaunch ros_master_test lightrover_gmapping.launch
#   4) Ctrl+C 时一并 kill
################################################################################

RASPI_HOST="pi-desktop.local"
RASPI_USER="pi"
RASPI_LAUNCH="roslaunch lightrover_ros gmapping.launch"
PC_LAUNCH="roslaunch ros_master_test lightrover_gmapping.launch"

PC_ROS_SETUP="source ~/catkin_ws/devel/setup.bash"
RASPI_ROS_SETUP="source /home/pi/catkin_ws/devel/setup.bash"

trap on_finish INT TERM

on_finish(){
  echo "[SCRIPT] Cleaning up..."
  [ ! -z "$SSH_PID" ] && kill $SSH_PID
  [ ! -z "$PC_PID" ] && kill $PC_PID
  wait
  echo "[SCRIPT] Done."
  exit 0
}

echo "[SCRIPT] [1/4] SSH to RPi: $RASPI_LAUNCH"
ssh -t $RASPI_USER@$RASPI_HOST "
  source /opt/ros/noetic/setup.bash
  $RASPI_ROS_SETUP
  echo '[RPi] RUN: $RASPI_LAUNCH'
  $RASPI_LAUNCH
" &
SSH_PID=$!

sleep 5

echo "[SCRIPT] [2/4] Local PC launch: $PC_LAUNCH"
bash -c "
  $PC_ROS_SETUP
  $PC_LAUNCH
" &
PC_PID=$!

echo "[SCRIPT] [3/4] Running. Press Ctrl+C to stop."
wait

