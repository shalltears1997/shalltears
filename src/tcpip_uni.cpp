#if !defined(_WIN32) && !defined(_WIN64)

#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> //for sockaddr_in, sockaddr
#include <arpa/inet.h> //for htons, inet_addr
#include <stdio.h>
#include <stdlib.h> // for exit
#include <stdio.h>
#include <string.h> //for memset
#include <unistd.h>
#define BACKLOG 4 //Maximun size of cue



int setTimeOut(int sockSERrv, double milliseconds)
{
  fd_set rfds;
  struct timeval tv;

  FD_ZERO(&rfds);
  FD_SET(sockSERrv, &rfds);

  tv.tv_sec = 0;
  tv.tv_usec = milliseconds * 1000;

  return select(sockSERrv+1, &rfds, NULL, NULL, &tv);
}

void closesocket(int sockSERrv)
{
  close(sockSERrv);
}

void TCPErr(const char* errmessage)
{
  perror(errmessage);
  sleep(1);
  exit(1);
}

int CreateTCPClientSocket(unsigned short port, const char* ip)
{
  int sockCLIrv, connectrv;
  struct sockaddr_in cliTCP;
  cliTCP.sin_family = AF_INET;
  cliTCP.sin_port = htons(port);
  cliTCP.sin_addr.s_addr = inet_addr(ip);
  memset(&(cliTCP.sin_zero), '\0', 8);

  //prepare socket
  sockCLIrv = socket(AF_INET, SOCK_STREAM, 0);
  if(sockCLIrv < 0) TCPErr("Client_socket_error");

  //connect
  connectrv = connect(sockCLIrv, (struct sockaddr*) &cliTCP, sizeof(struct sockaddr));
  if(connectrv != 0) TCPErr("Client_connect_error_zps");

  fflush(NULL);
  return sockCLIrv;
}

int CreateTCPServerSocket(unsigned short port)
{
  int sockSERrv, bindrv, listenrv;
  struct sockaddr_in servTCP;

  //prepare socket
  sockSERrv = socket(AF_INET, SOCK_STREAM, 0);
  if(sockSERrv < 0) TCPErr("Server_socket_error");

  // Set option -> server socket
  int opt = 1;
  if ((setsockopt( sockSERrv, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int))) != 0 )
    TCPErr("Server_sockopt_error");

  memset(&servTCP, 0, sizeof(servTCP));

  servTCP.sin_family = AF_INET;

  servTCP.sin_port = htons(port);

  servTCP.sin_addr.s_addr = htonl(INADDR_ANY);

  //bind
  bindrv = bind(sockSERrv, (struct sockaddr*) &servTCP, sizeof(struct sockaddr));
  if(bindrv != 0) TCPErr("Server_bind_error");

  //listen
  listenrv = listen(sockSERrv, SOMAXCONN);
  if(bindrv != 0) TCPErr("Server_listen_error");

  return sockSERrv;
}

//int AcceptTCPServer(int sockSERrv)
// Windowsの関数 SOCKET AcceptTCPClient(int sockSERrv) に倣って、名前を変更した。↑旧・↓新
int AcceptTCPClient(int sockSERrv)
{
  int acceptrv;
  struct sockaddr_in echocliTCP;
  int clilen;
  clilen = sizeof(echocliTCP);

  //accept
  acceptrv = accept(sockSERrv, (struct sockaddr*) &echocliTCP, (socklen_t*) &clilen);
  if(acceptrv < 0) TCPErr("Accept_error!!!");

  printf("connected to %s\n", inet_ntoa(echocliTCP.sin_addr));

  return acceptrv;
}

#endif
