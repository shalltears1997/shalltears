//windows環境用TCP/IP関数 written by Hiromu Kobayashi(2010.11)

#if defined(_WIN32) || defined(_WIN64)

//#include "c_TCPIPTools_Win.hpp"
#pragma comment(lib, "wsock32.lib")

#include <cstdio>
#include <winsock2.h> //#include <winsock2.h>は書く場所(順番)が大事．

//#include <windows.h>			// おそらく不要になったやつ
//#pragma comment(lib, "ws2_32.lib")	// おそらく不要になったやつ

//エラー用
void TCPErr(const char* errmessage)
{
	perror(errmessage);
	exit(1);
}

//サーバソケット生成
SOCKET CreateTCPServerSocket(unsigned short port)
{
	SOCKET sock;
	struct sockaddr_in servAddr;
	short opt = 1;

	WSADATA wsaData;
	int wsaret = WSAStartup(0x101, &wsaData);
	if(wsaret) return 0;

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(port);
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	//prepare socket
	if( (sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) TCPErr("Server_socket_error");;

	// Set option -> server socket
    //BOOL opt = 1;
    if ((setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(int))) != 0 )
      TCPErr("Server_sockopt_error");

	//bind
	if( (bind(sock, (struct sockaddr*) &servAddr, sizeof(struct sockaddr))) != 0) TCPErr("Server_bind_error");

	//listen
	if( (listen(sock, SOMAXCONN)) != 0) TCPErr("Server_listen_error");

	printf("socket is created\n");

	return sock;
}

//サーバアクセプト処理
SOCKET AcceptTCPClient(int sockSERrv)
{
  SOCKET accSock;
  struct sockaddr_in echocliTCP;
  
  int clilen = sizeof(echocliTCP);

	printf("socket is waiting to accept\n");

  //accept
	//if( (accSock = accept(sockSERrv, (struct sockaddr*) &echocliTCP, (socklen_t*) &clilen)) < 0)
		//TCPErr("Accept_error");
	if( (accSock = accept(sockSERrv, (struct sockaddr*) &echocliTCP, (int*) &clilen)) < 0)
		TCPErr("Accept_error");
  
  // if there is no problem until here, TCP/IP connection has done.
	printf("accepted(%s)\n", inet_ntoa(echocliTCP.sin_addr) );

  return accSock;
}

//クライアントソケット生成
SOCKET CreateTCPClientSocket(unsigned short port, const char* ip)
{
	SOCKET sockCLI;
	struct sockaddr_in cliAddr;

	WSADATA wsaData;
	int wsaret = WSAStartup(0x101, &wsaData);
	if(wsaret) return 0;

	cliAddr.sin_family = AF_INET;
	cliAddr.sin_port = htons(port);
	cliAddr.sin_addr.s_addr = inet_addr(ip);
	memset(&(cliAddr.sin_zero), '\0', 8);

	//prepare socket
	if( (sockCLI = socket(AF_INET, SOCK_STREAM, 0)) < 0) TCPErr("Client_socket_error");

	//connect
	if( (connect(sockCLI, (struct sockaddr*) &cliAddr, sizeof(struct sockaddr))) != 0) TCPErr("Client_connect_error");

	fflush(NULL);
	return sockCLI;
}

#endif