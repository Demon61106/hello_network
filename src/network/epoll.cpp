#include "epoll.h"

#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>

#include <pthread.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_EVENT_NUMBER 1024
#define BUFFER_SIZE      1024

int SetNonBlocking(int fd) 
{
    int oldOpt = fcntl(fd, F_GETFL);
    int newOpt = oldOpt | O_NONBLOCK;
    fcntl(fd, F_SETFL, newOpt);
    return oldOpt;
}

int AddFd(int epollFd, int fd, bool enable_et) 
{
    printf("epoll add: add fd = %d, fd = %d\n", epollFd, fd);
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN;
    if (enable_et) {
        event.events |= EPOLLIN;
    }
    epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &event);
    SetNonBlocking(epollFd);
    return 0;
}

void Lt(epoll_event* events, int number, int epollFd, int listenFd) {
    char buf[BUFFER_SIZE];
    for (int i = 0; i < number; ++i) {
        int sockFd = events[i].data.fd;
        if (sockFd == listenFd) {
            struct sockaddr_in clientAddr;
            socklen_t clientAddrLen = sizeof(clientAddr);
            int connFd = accept(listenFd, (struct sockaddr*)&clientAddr, &clientAddrLen);
            printf("accept a connect\n");
            AddFd(epollFd, connFd, false);
        } else if (events[i].events & EPOLLIN) {
            printf("events trigger onces\n");
            memset(buf, '\0', BUFFER_SIZE);
            int ret = recv(sockFd, buf, BUFFER_SIZE-1, 0);
            if (ret <= 0) {
                close(sockFd);
                continue;
            }
            printf("get %d byte content: %s\n", ret, buf);
        } else {
            printf("something else happended\n");
        }
    }
}

void Et(epoll_event* events, int number, int epollFd, int listenFd) 
{
    char buf[BUFFER_SIZE];
    for (int i = 0; i < number; ++i) {
        int sockFd = events[i].data.fd;
        if (sockFd == listenFd) {
            struct sockaddr_in clientAddr;
            socklen_t clientAddrLen = sizeof(clientAddr);
            int connFd = accept(listenFd, (struct sockaddr*)&clientAddr, &clientAddrLen);
            printf("accept a connect\n");
            AddFd(epollFd, connFd, true);
        } else if (events[i].events & EPOLLIN) {
            printf("events trigger once\n");
            while (1) {
                memset(buf, '\0', BUFFER_SIZE);
                int ret = recv(sockFd, buf, BUFFER_SIZE-1, 0);
                if (ret < 0) {
                    if ((errno == EAGAIN || errno == EWOULDBLOCK)) {
                        printf("read later\n");
                        break;
                    }
                    close(sockFd);
                    break;
                } else if(ret == 0) {
                    close(sockFd);
                } else {
                    printf("get %d bytes of content: %s\n", ret, buf);
                }
            }
        } else {
            printf("something else happended \n");
        }
    }
}

int Epoll() 
{
    char ip[] ="0.0.0.0";

    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_family = AF_INET;
    address.sin_port = htons(5022);

    int listenFd = socket(PF_INET, SOCK_STREAM, 0);
    if (listenFd < 0) {
        printf("socket error\n");
        return -1;
    }

    int ret = bind(listenFd, (struct sockaddr*)&address, sizeof(address));
    if (ret < 0) {
        printf("bind error\n");
        return -1;
    }

    ret = listen(listenFd, 5);
    if (ret < 0) {
        printf("listen failed\n");
        return -1;
    }

    printf("socket init success\n");
    epoll_event events[MAX_EVENT_NUMBER];
    int epollFd = epoll_create(1);
    if (epollFd < 0) {
        printf("epoll_create error\n");
        return -1;
    }
    printf("epoll init success\n");
    AddFd(epollFd, listenFd, false);
    while (1) {
        printf("epoll running\n");
        int ret = epoll_wait(epollFd, events, MAX_EVENT_NUMBER, -1);
        if (ret < 0 ) {
            printf("epoll wait failed");
            break;
        }
        printf("epoll event trigger\n");
        Lt(events, ret, epollFd, listenFd);
    }
    close(listenFd);
    return 0;
}

void* EpollFunc(void *p) 
{
    Epoll();
    return NULL;
}

int EpollTest() 
{
   pthread_t  tid;
   int ret = pthread_create(&tid, NULL, EpollFunc, NULL);
   if (ret < 0) {
       printf("pthread create failed");
       return -1;
   }
   pthread_join(tid, NULL);
   return 0;
}

