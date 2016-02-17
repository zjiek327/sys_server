#include "inc/sys_event.h"

int dev_read(int fd, void *buf, size_t count)
{
	int ret = -1;

	do{
		ret = read(fd, buf, count);
	}while(ret < 0 && EINTR == errno);
	if(ret <= 0)
		goto returnoff;
	else{
		goto returnon;
	}

returnoff:
	perror("--read");
	CLOSEFD(fd);
returnon:
	return ret;
}

int dev_write(int fd, void *buf, size_t count)
{
	int ret = -1;

	do{
		ret = write(fd, buf, count);
	}while(ret < 0 && EINTR == errno);
	if(ret <= 0)
		goto returnoff;
	else{
		goto returnon;
	}

returnoff:
	perror("--write");
	CLOSEFD(fd);
returnon:
	return ret;
}

void dev_stop(uint8 *status)
{
	DEBUG("Get the device status: %d\n", *status);
	*status = 0;
	DEBUG("Get the device status: %d\n", *status);
}

