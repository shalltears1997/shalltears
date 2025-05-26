#if !defined(_WIN32) || defined(_WIN64)

#pragma once

int setTimeOut(int sockSERrv, double milliseconds);
void closesocket(int sockSERrv);
void TCPErr(const char* errmessage);
int CreateTCPClientSocket(unsigned short port, const char* ip);
int CreateTCPServerSocket(unsigned short port);
//int AcceptTCPServer(int sockSERrv)
// Windowsの関数 SOCKET AcceptTCPClient(int sockSERrv) に倣って、名前を変更した。↑旧・↓新
int AcceptTCPClient(int sockSERrv);

#endif
