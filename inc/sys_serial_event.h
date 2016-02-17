
#ifndef __ELEX_SERIAL_EVENT_H__
#define __ELEX_SERIAL_EVENT_H__

/**
* @file    	sys_serial_event.h
* @brief   	The api for system server.
* @details 	The api for system server.
*		   	This is main function entrance at system server program.
* @author  	(JT)
* @date    	2015/10/30
* @version 	1.0
*
* @Copyright (c) 2015
*/

#include "sys_server.h"
#include "sys_event.h"
#include "sys_task.h"

static int serial_errno;

extern int system_serial_init(serial_manage *serial_info);
extern void *event_serial(void *arg);
/* initial device pt1000 serial */
extern void pt1000_serial_info_init(void *arg);
extern int open_serial(char *devname);
extern int set_serial(int fd, void *arg);
extern int start_serial(void *arg);


#endif
