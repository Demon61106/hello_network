#ifndef __SOCKET_H__
#define __SOCKET_H__

void* TcpServerRun(void* arg);
void TcpServer();

void* TcpClientRun(void* arg);

void TcpClient();
#endif // __SOCKET_H__