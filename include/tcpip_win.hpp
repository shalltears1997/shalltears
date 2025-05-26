#if defined(_WIN32) || defined(_WIN64)

#pragma once

//socketを使うには下記のようにライブラリを追加する
//プロジェクト->OOのプロパティ->構成プロパティ->リンカ->入力->追加の依存ファイル->ws2_32.lib

//#include <stdio.h>
#include <winsock2.h> //#include <winsock2.h>は書く場所(順番)が大事

//クライアントの数を制限する場合(listenに使う)
//const int BACKLOG = 4;

//TCP/IP通信用関数のプロトタイプ宣言
void TCPErr(const char* errmessage);
SOCKET CreateTCPServerSocket(unsigned short port);
SOCKET AcceptTCPClient(int sockSERrv);
SOCKET CreateTCPClientSocket(unsigned short port, const char* ip);

#endif