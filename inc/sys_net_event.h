#ifndef __ELEX_NET_EVENT_H__
#define __ELEX_NET_EVENT_H__

/**
* @file    	sys_net_event.h
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
#include "sys_task.h"

/* 涉及到具体数据部分解析 */
extern void *event_deal(void *arg);
/* 网络时间监听处理 */
extern net_manage *event_ethernet_listening(net_manage *net_mg, char *eth);
/* 网络事件监听并分配任务*/
extern int event_ethernet_deal(system_server *sys);
extern void *event_ethernet(void *arg);


#endif
