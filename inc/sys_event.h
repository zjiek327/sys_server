#ifndef __ELEX_EVENT_H__
#define __ELEX_EVENT_H__

#include "vtype.h"
#include "debug.h"

/**
* @file    sys_event.h
* @brief   The structure of event.
* @details This file descripte event operate type and event
* 		   object.
*
* @author  (JT)
* @date    2015/11/12 
* @version 1.0
*
* @Copyright (c) 2015
*/

#define CLOSEFD(fd) \
	do{\
		if(fd > 0){ close(fd); fd = 0; }\
	}while(0)

enum EVOP_SETTING_MODE
{
	ELEXSMRT_EVOP_SET_NONBLOCKING = 0x1, ///< set fd non-blocking
	ELEXSMRT_EVOP_SET_SOREUSEADDR, ///< set ip address reuse

	// ......
	
	NONE
};

/**
 * This structure is descripte the same operate of all device,
 * these function is a abstract operate for different device, 
 * it point to real device whom has event occurs.
 * For example, if 'eth0' has read event, then the @evop_read
 * will do read data from 'eth0', because @evop_read point to
 * 'eth0' read operate, this system start will init all devices
 * operate method, like read/write/init etc.
 */
typedef struct smrt_event_op
{
	void (*evop_init)(void *);
	int (*evop_start)(void *);
	void (*evop_stop)(uint8 *);
	int (*evop_open)(char *);
	int (*evop_read)(int, void *, size_t);
	int (*evop_write)(int, void *, size_t);
	int (*evop_set)(int, void *);
}smrt_event_op;





extern int dev_read(int fd, void *buf, size_t count);
extern int dev_write(int fd, void *buf, size_t count);
extern void dev_stop(uint8 *status);



#endif
