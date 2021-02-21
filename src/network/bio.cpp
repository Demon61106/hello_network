#include "bio.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#include <sys/uio.h>
#include <sys/mman.h>
#include <sys/stat.h>

int RWVTest() {
    ReadV();
    WriteV();
}

int WriteV()
{
    char *str0 = "hello ";
    char *str1 = "world\n";
    struct iovec iov[2];
    ssize_t nwritten;

    iov[0].iov_base = str0;
    iov[0].iov_len = strlen(str0);
    iov[1].iov_base = str1;
    iov[1].iov_len = strlen(str1);

    nwritten = writev(STDOUT_FILENO, iov, 2);
    printf("%ld bytes written.\n", nwritten);
    return 0;
}

int ReadV()
{
    char buf1[8] = {0};
    char buf2[8] = {0};
    struct iovec iov[2];
    ssize_t nread;

    iov[0].iov_base = buf1;
    iov[0].iov_len = sizeof(buf1) - 1;
    iov[1].iov_base = buf2;
    iov[1].iov_len = sizeof(buf2) - 1;

    nread = readv(STDIN_FILENO, iov, 2);
    printf("%ld bytes read.\n", nread);
    printf("buf1: %s\n", buf1);
    printf("buf2: %s\n", buf2);

    return 0;
}

int MMapTest()
{
    int fd = 0;
    char *ptr = NULL;
    struct stat buf = {0}; 
 
    if ((fd = open("test.txt", O_RDWR)) < 0)
    {
        printf("open file error\n");
        return -1;
    }
 
    if (fstat(fd, &buf) < 0)
    {
        printf("get file state error:%d\n", errno);
        close(fd);
        return -1;
    }
 
    ptr = (char *)mmap(NULL, buf.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED)
    {
        printf("mmap error: %d\n", errno);
        close(fd);
        return -1;
    }
 
    close(fd);
 
    printf("length of the file is : %d\n", buf.st_size);
    printf("the content is : %s\n", ptr);
 
    ptr[3] = 'a';
    printf("the new content is : %s\n", ptr);
    munmap(ptr, buf.st_size);
    return 0;
}