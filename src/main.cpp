#include <iostream>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include "./network/endian.h"
#include "./network/socket.h"
#include "./network/pipe.h"
#include "./network/bio.h"
#include "./network/epoll.h"

int main(int argc, char *argv[])
{

	// PipeTest();
	// Dup2Test();
	// RWVTest();
	// MMapTest();
	EpollTest();

	while (1)
	{
		sleep(1);
	}

	return 0;
}
