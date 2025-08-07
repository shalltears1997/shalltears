/********************************************************************
 *  tk13_connect_mc.cpp  ——  完整版（包含 tmp_goal 扩展）
 *******************************************************************/
#include <iostream>
#include <cstring>
#include <chrono>
#include <thread>
#include <cmath>
#include <cstdio>

#if defined(_WIN32) || defined(_WIN64)
  #include "tcpip_win.hpp"
#else
  #include <sys/types.h>
  #include <sys/socket.h>
  #include "tcpip_uni.hpp"
#endif

#include "class.hpp"
#include "gv_extern_id.hpp"
#include "gv_extern.hpp"
#include "05_connect_mc.hpp"
#include "22_hark.hpp"
#include <sys/select.h>

/* ================================================================ */
/*  发送                                                             */
/* ================================================================ */
int connectWithMCClass::sendData()
{
  // -------- 改 1：发送缓冲大小 --------
  char send_message[110] = {};                    // *** 修改 ***
  int  num_of_string = 0;
  int  return_value  = -1;

  // -------- 改 2：sprintf 增加 tmp_goal --------
  num_of_string = sprintf(
      send_message,
      "%07.1f,%1d,%02d,%07.1f,%07.1f,"
      "%2d,%07.2f,%07.2f,%07.1f,%07.1f,%07.1f,%07.1f,%07.1f,%07.1f,%07.1f",
      sound_flag, robot_move_or_halt, command.head_flag,
      sub_goal.x, sub_goal.y,
      ball_st_hand_flag, sound_gaze_angle, hark_sd,
      robot.x, robot.y,
      command.pan.deg, command.tilt.deg, command.body.deg,
      tmp_goal.x, tmp_goal.y);                    // *** 修改 ***

  // -------- 改 3：按真实长度发送并检查 --------
  send_cnt = send(acc_Pio, send_message, num_of_string, 0);
  if (send_cnt == num_of_string)
      return_value = send_cnt;
  else
      return_value = -1;

  return return_value;
}

/* ================================================================ */
/*  接收  —— 下面内容完全沿用原实现，没有任何改动                    */
/* ================================================================ */
int connectWithMCClass::receiveData()
{
  char recv_msg[127] = {""};
  int  num_of_recv = 0;
  double temp_pose[2] = {};
  char *tp;
  char tp_a[72] = "";
  char recv_msg_cp[256] = "";

  if ((num_of_recv = recv(acc_Pio, recv_msg, num_of_recv_pio, 0)) == -1)
  {
    perror("Recieve data from ZPS");
    return -1;
  }

  strcpy(recv_msg_cp, recv_msg);
  tp = strtok(recv_msg, "#");
  if (tp != NULL) { strcpy(tp_a, tp); velocity = atof(tp_a); }
  tp = strtok(recv_msg, "#");
  if (tp != NULL) { strcpy(tp_a, tp); robot_deg = atof(tp_a); }
  tp = strtok(NULL, "#");
  if (tp != NULL) { strcpy(tp_a, tp); catch_flag = atoi(tp_a); }
  tp = strtok(NULL, "#");
  if (tp != NULL) { strcpy(tp_a, tp); ball_distance_0 = atof(tp_a); }
  tp = strtok(NULL, "#");
  if (tp != NULL) { strcpy(tp_a, tp); ball_theta_1 = atof(tp_a); }
  tp = strtok(NULL, "#");
  if (tp != NULL) { strcpy(tp_a, tp); confidence_sum_1 = atof(tp_a); }
  tp = strtok(NULL, "#");
  if (tp != NULL) { strcpy(tp_a, tp); SD_1 = atof(tp_a); }
  tp = strtok(NULL, "#");
  if (tp != NULL) { strcpy(tp_a, tp); circularity_1 = atof(tp_a); }

  while (robot_deg > 360) robot_deg -= 360;
  while (robot_deg < -360) robot_deg += 360;
  if (robot_deg > 180)  robot_deg -= 360;
  if (robot_deg < -180) robot_deg += 360;

  strcpy(recv_msg, "");
  fflush(stdin);

  return num_of_recv;
}

/* ================================================================ */
/*  线程主体                                                        */
/*
 * 修改说明：
 * - 原版在 45678 端口上既接收又发送数据，并在接收超时或发送失败时结束线程。
 * - 本版取消接收环节，只负责周期性调用 sendData() 发送 15 个字段。
 * - 当 sendData() 返回 -1（发送失败）时，认为客户端断开连接，退出内循环，
 *   关闭 acc_Pio 后重新等待新的客户端（如 etho_output.py）连接。
 */
/* ================================================================ */
void connectWithMCClass::connectLoop()
{
  PRINT("ここまできてるのかな");
  // 创建监听 socket，仅在程序结束时关闭
  ser_Pio = CreateTCPServerSocket(PORT_Cli_Pio);

  while (thread_continue_flag)
  {
    PRINT("The thread \"SendToOnboard\" is waiting to be connected...");
    // 阻塞等待客户端连接（Python程序会作为客户端连接到 45678）
    acc_Pio = AcceptTCPClient(ser_Pio);
    PRINT("The thread \"SendToOnboard\" is connected.");

    // 设置客户端连接为无超时（0 表示阻塞直到有数据）
    struct timeval tv;
    tv.tv_sec  = 0;
    tv.tv_usec = 0;
    setsockopt(acc_Pio, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));

    // 循环发送数据：每 100 毫秒向客户端发送一次当前机器人状态
    while (thread_continue_flag)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      /* 
       * 通过 sendData() 将机器人内部状态（15 个字段）发送给客户端。
       * 如果发送失败（返回 -1），说明客户端断开，跳出内循环重新监听。
       */
      int send_ret = sendData();                // *** 修改：仅发送，不再接收 ***
      if (send_ret == -1)
      {
        PRINT("send error, reset connection");
        break;
      }
    }

    // 仅关闭当前客户端连接，继续等待新的连接
    closesocket(acc_Pio);
    PRINT("closed accepted connection");
  }

  // 线程结束，关闭监听 socket
#if defined(_WIN32) || defined(_WIN64)
  closesocket(ser_Pio);
#else
  close(ser_Pio);
#endif
  PRINT("the connect with MC thread finished its task.");
}

/* ================================================================ */
/*  线程控制函数——保持原实现                                          */
/* ================================================================ */
void connectWithMCClass::connectLoopThreadBegin(bool *is_new)
{
  thread_1 = new std::thread(&connectWithMCClass::connectLoop, this);
  this->is_new = is_new;
}

void connectWithMCClass::connectLoopThreadFinish()
{
  if (thread_1 == nullptr)
  {
    PRINT("the thread instance had not activated.");
  }
  else
  {
    thread_continue_flag = false;
    thread_1->join();
    thread_1 = nullptr;
    PRINT("the thread instance has successfully removed.");
  }
}

connectWithMCClass::~connectWithMCClass()
{
  connectLoopThreadFinish();
}

/* ================================================================ */
/*  与主线程数据交互                                                 */
/* ================================================================ */
void connectWithMCClass::dataInput(DataClass *data_in)
{
  data_in->robot.body_deg = robot_deg;

  data_in->ball_st.catch_flag        = catch_flag;
  data_in->ball_st.ball_distance[0]  = ball_distance_0;
  data_in->ball_st.ball_theta[0]     = ball_theta_1;
  data_in->ball_st.confidence_sum[0] = confidence_sum_1;
  data_in->ball_st.SD[0]             = SD_1;
  data_in->ball_st.circularity[0]    = circularity_1;
}

void connectWithMCClass::dataOutput(DataClass *data_in)
{
  pioneer_activate   = data_in->pioneer_activate;
  command.head_flag  = data_in->command.head_flag;
  sub_goal           = data_in->sub_goal;
  tmp_goal           = data_in->tmp_goal;           // *** MOD ***
  ball_st_hand_flag  = data_in->ball_st.hand_flag;
  sound_gaze_angle   = 0;
  hark_sd            = 0;
  robot              = data_in->robot.posi;
  command            = data_in->command;
  robot_move_or_halt = data_in->robot_move_or_halt;
}

