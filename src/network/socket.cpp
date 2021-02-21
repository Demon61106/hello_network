#include "socket.h"

#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <pthread.h>

int NetworkTest() {
	pthread_t tid1;
	int ret = pthread_create(&tid1, NULL, TcpServerRun, NULL);
	if (ret != 0) {
		printf("can't create thread: %s\n", strerror(ret));
		return -1;
	}

	sleep(1);

	pthread_t tid2;
	ret = pthread_create(&tid2, NULL, TcpClientRun, NULL);
	if (ret != 0) {
		printf("can't create thread: %s\n", strerror(ret));
		return -1;
	}
    return 0;
}

void* TcpServerRun(void* arg) {
    TcpServer();
}

void* TcpClientRun(void* arg) {
    TcpClient();
}

void TcpClient() {
    int fd = socket(PF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        printf("socket error: %d\n", errno);
        return ;
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
    serverAddr.sin_port = htons(2021);

    int ret = connect(fd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (ret != 0) {
        printf("connect error: %d\n", errno);
        return ;
    }

    char sendBuf[512] = "hello server!";
    send(fd, sendBuf, strlen(sendBuf), 0);
    close(fd);
}

void TcpServer() {
    int fd = socket(PF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        printf("socket error: %d\n", errno);
        return ;
    }

    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, "0.0.0.0", &address.sin_addr);
    address.sin_port = htons(2021);

    int ret = bind(fd, (struct sockaddr*)&address, sizeof(address));
    if (ret == -1) {
        printf("bind error: %d", errno);
        return ;
    }

    ret = listen(fd, 3);
    if (ret == -1) {
        printf("listen error: %d", errno);
        return ;
    }

    struct sockaddr_in client;
    socklen_t clientAddrLen = sizeof(client);
    int connfd = accept(fd, (struct sockaddr*)&client, &clientAddrLen);
    if (connfd < 0) {
        printf("accept error: %d\n", errno);
        return ;
    }
    char clientAddr[INET_ADDRSTRLEN];
    printf(" accept success. client is %s:%d\n", inet_ntop(AF_INET, &client.sin_addr, clientAddr, INET_ADDRSTRLEN), ntohs(client.sin_port));

    char recvBuf[512];
    recv(connfd, recvBuf, sizeof(recvBuf), 0);
    printf("recv: %s\n", recvBuf);
    // close(fd);
    return ;
}