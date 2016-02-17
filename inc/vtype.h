/**
* @file    	vtype.h
* @brief   	typedef definition
* @details 	typedef definition
* @author  	(JT)
* @date    	2015/10/30
* @version 	1.0
*
* @Copyright (c) 2015
*/


#ifndef _STREAMTYPE_H
#define _STREAMTYPE_H

#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <net/if.h>
#include <net/route.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <alsa/iatomic.h>
#include <termios.h>
#include <fcntl.h>
#include <assert.h>



#if 1
typedef unsigned char uchar;
typedef unsigned int  uint32;
typedef int  int32;
typedef unsigned short uint16;
typedef short int16;
typedef unsigned long ulong;

typedef int sockfd_t;
typedef int file_t;
typedef int return_t;

#endif

typedef unsigned long long int uint64;
typedef unsigned char uint8;

/// address info of device
typedef struct socketaddr_in addr_t;

#endif
