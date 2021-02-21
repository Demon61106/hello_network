#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>

int PipeTest() 
{
    int fd[2];

    int* readFd = &fd[0];
    int* writeFd = &fd[1];

    int ret = pipe(fd);
    if (ret == -1) {
        printf("pipe error\n");
        return -1;
    }

    char msg[] = "hello world";
    char rbuf[100] = {0};
    pid_t pid = fork();
    if (pid == -1) {
        printf("fork error\n");
        return -1;
    } else if (pid == 0){
        close(*readFd);
        ret = write(*writeFd, msg, strlen(msg));
        return 0;
    } else {
        close(*writeFd);
        int nbytes = read(*readFd, rbuf, sizeof(rbuf));
        printf("receive %d bytes buf: %s\n", nbytes, rbuf);
    }
    return 0;
}

int DupTest() 
{
    int fd = open("hello", O_CREAT|O_RDWR|O_TRUNC, S_IRUSR|S_IWUSR);
    if(fd < 0)
    {
        printf("Open Error!!\n");
        return 0;
    }


    int nfd = dup(fd);
    if(nfd < 0)
    {
        printf("Error!!\n");
        return 0;
    }

    char buf[1000];
    int n;

    while((n = read(STDIN_FILENO, buf,1000)) > 0)
    {
        if(write(nfd, buf, n) != n)
        {
            printf("Write Error!!\n");
            return 0;
        }
    }
    return 0;
}

int Dup2Test() 
{
    int fd = open("dup2.txt", O_CREAT|O_RDWR|O_TRUNC,S_IRUSR|S_IWUSR);
    if(fd < 0)
    {
        printf("Open Error!!\n");
        return 0;
    }
    printf("111 fd:%d STDOUT_FILENO:%d\n", fd, STDOUT_FILENO);
    int nfd = dup2(fd, STDOUT_FILENO);
    if(nfd < 0)
    {
        printf("Error!!\n");
        return 0;
    }
    
    char buf[1000];
    int n;
    
    while((n = read(STDIN_FILENO, buf, 1000)) > 0)
        if(write(nfd, buf, n) != n)
        {
            printf("Write Error!!\n");
            return 0;
        }
    return 0;
}