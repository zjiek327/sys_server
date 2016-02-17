#ifndef __ELEX_SMRT_H__
#define __ELEX_SMRT_H__

/*
 *	uC - unsigned char
 *	gC - char
 *	uI - unsigned int
 *	gI - int
 *	uL - unsigned long
 *	gL - long
 *	uS - unsigned short
 *	gS - short
 *	sT - struct
 *
 *	enum - CAPITAL LETTER
 *	macro - prefix(ELEXSMRT_ or ELEX_) and CAPITAL LETTER
 */

/**
* @file    sys_smrt.h
* @brief   The general infomation for system server.
* @details The general infomation for system server.
*
*		   @USER_PERM:
*				user permission in this system
*		   @user_info:
*				the basicly infomation of users, they are also have name, password etc.
*		   
*		   more details will be add......
*
* @author  (JT)
* @date    2015/10/26 
* @version 1.0
*
* @Copyright (c) 2015
*/

#define __IPV4__ 1

#if __IPV4__
#define ELEXSMRT_IPV4_SIZE 16
#endif
#if __IPV6__
#define ELEXSMRT_IPV6_SIZE 128
#endif

#define ELEXSMRT_MAC_SIZE 48
#define ELEXSMRT_NETCARD_MAXNUM 4
#define ELEXSMRT_USER_MAXNUM	32
#define ELEXSMRT_PASSWD_SIZE	32
#define ELEXSMRT_NAME_SIZE	256
#define ELEXSMRT_TPOOL_MAXNUM 128
#define ELEXSMRT_TASK_MAXNUM 2048

#define DATA_SIZE 1024*8

#define ELEXSMRT_REQ_DATA_LENGTH 1024

#define ELEXSMRT_LISTEN_PORT 8858
#define ELEXSMRT_HTTP_PORT 8859
#define ELEXSMRT_ETH_NAME "eth0"

#define ELEXSMRT_INFACE_FIFONAME "elexfifo"

#define EDEVINIT -1
#define EDEVOPEN -2
#define EDEVSET  -3

#define FREE(p) \
	do{ \
		if(p) free(p); \
		p = NULL; \
	}while(0)

enum USER_PERM
{
	ADMINISTRATOR,
	GUEST,
	GENERAL
};

/* users general attribute */
typedef struct user_info
{
	unsigned char uCname[ELEXSMRT_NAME_SIZE];
	unsigned char uCpassword[ELEXSMRT_PASSWD_SIZE];
	char gCpermission; ///< details see enum USER_PERM
	char gCstatus; ///< user status
}user_info;


#endif
