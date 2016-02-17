#ifndef __ELEX_SERVER_H__
#define __ELEX_SERVER_H__

/**
 * @file    sys_server.h
 * @brief   Global object management for system server.
 * @details Global object management for system server.
 *
 *		   @DEV_TYPE:
 *				device type, for examper, lights, doors, tv, etc.
 *				This enum descripte all of the device of home controlor jiaju.
 *		   @DEV_OPERATE:
 *				maybe the operation for every device is not different, 
 *				if the enum's member be set means that operation will be occus.
 *		   @DEV_PERMISSION:
 *				permission of the device.
 *		   @dev_basic:
 *				the basicly infomation of devices, they are also have name, id etc.
 *
 * @author  (JT)
 * @date    2015/10/26 
 * @version 1.0
 *
 * @Copyright (c) 2015
 */

#include <evhttp.h>
#include <event.h>
#include <sys/utsname.h>
#include "sys_smrt.h"
#include "sys_dev.h"
#include "sys_entype.h"
#include "vtype.h"
#include "debug.h"
#include "dlist.h"
#include "sys_event.h"


#define __HTTP_ENABLE__ 1
#define __ETHERNET__ 1
#define __SERIAL__ 1



/**
 *      struct utsname {
 *          char sysname[];     ///< Operating system name (e.g., "Linux") 
 *			char nodename[];    ///< Name within "some implementation-defined network" 
 *			char release[];     ///< OS release (e.g., "2.6.28") 
 *			char version[];     ///< OS version 
 *			char machine[];     ///< Hardware identifier 
 *			#ifdef _GNU_SOURCE
 *			char domainname[];  ///< NIS or YP domain name 
 *			#endif
 *		};
 *
 *		The length of the arrays in a struct utsname is unspecified (see NOTES); the fields are terminated by a null byte ('\0').
 */
typedef struct system_info
{
	char gCsys_platform[ELEXSMRT_NAME_SIZE]; ///< operation of system
	char gChostname[ELEXSMRT_NAME_SIZE]; ///< host name
	char gCversion[ELEXSMRT_NAME_SIZE];  ///< OS version
	char gCrelease[ELEXSMRT_NAME_SIZE];  ///< OS release (e.g., "2.6.28")
	char gCmachine[ELEXSMRT_NAME_SIZE];  ///< Hardware identifier 
	char gClevel; ///< level of performance

	// more and fix me...

}system_info;

typedef struct bluetooth_manage
{
	uint8 uCrunning; ///< 1: work well     0:work broken
}bluetooth_manage;

typedef struct net_dev
{
	char gCintername[ELEXSMRT_NAME_SIZE];	
	char gCmac[ELEXSMRT_MAC_SIZE];
#if __IPV4__
	char gCipaddr[ELEXSMRT_IPV4_SIZE];
	char gCnetmask[ELEXSMRT_IPV4_SIZE];
	char gCgateway[ELEXSMRT_IPV4_SIZE];
	char gCbcastaddr[ELEXSMRT_IPV4_SIZE];
#endif
#if __IPV6__
	char gCipaddr[ELEXSMRT_IPV6_SIZE];
	char gCnetmask[ELEXSMRT_IPV6_SIZE];
	char gCgateway[ELEXSMRT_IPV6_SIZE];
	char gCbcastaddr[ELEXSMRT_IPV6_SIZE];
#endif
}net_dev;

typedef struct rq_queue
{
	int gIsock;
	char gCdata[ELEXSMRT_REQ_DATA_LENGTH];
	void *owner;

	struct rq_queue *next;
}rq_queue;

typedef struct net_manage
{
	uint8 uCrunning;///< 1: work well     0:work broken
	net_dev sTnet;
	smrt_event_op sTnetop;
	int listen;
	struct event_base *net_base;
	struct event ev_listen;
	struct event ev_tout;
	struct timeval tv;

	struct evhttp *http_server;

	void *owner;
}net_manage;

typedef struct serial_manage
{
	uint8 uCrunning;///< 1: work well     0:work broken
	int gIhandle; ///< serial:file description

	char gCsname[ELEXSMRT_NAME_SIZE]; ///< Serial name, /dev/ttySx
	int gIspeed; ///< Speed
	int gIbits; ///< Trans
	int gIstop; ///< Stop
	int gIflowctrl; ///< Flow control
	char gCevent;///< Parity

	smrt_event_op op;
}serial_manage;


typedef struct system_server
{
	system_info sTsysinfo;
	user_info sTusers[ELEXSMRT_USER_MAXNUM];
	dev_manage sTsysdev; ///< global device management object, manage many devices
	net_manage sTsysnet[ELEXSMRT_NETCARD_MAXNUM]; ///< system network management object
	bluetooth_manage sTsysbt; ///< system bluetooth management object
	serial_manage sTsys_serial; ///< system serial management object
	bluetooth_manage sTsys_bt;  ///< system bluetooth management object

	pthread_t http_tid;

	void *(*do_task)(void *); ///< process request which from client.

}system_server;


extern int running;
extern system_server sys_server;

/**
 * system_info_init - Init system running environment information
 * 	
 * @param[in]  sysinfo System running environment infomation object.
 *
 * @return 0:success    -1:fail
 */
extern int system_info_init(system_info *sysinfo);

/**
 * system_net_init - Init system network information
 * 	
 * @param[in]  sysnet System network infomation object.
 *
 * @return 0:success    -1:fail
 */
extern int system_net_init(net_manage *sysnet);

/**
 * system_bt_init - Init system bluetooth information
 * 	
 * @param[in]  sysbt System bluetooth infomation object.
 *
 * @return 0:success    -1:fail
 */
extern int system_bt_init(bluetooth_manage *sysbt);

/**
 * system_serial_init - Init system serial information
 * 	
 * @param[in]  sys_serial System serial infomation object.
 *
 * @return 0:success    -1:fail
 */
extern int system_serial_init(serial_manage *sys_serial);










#endif
