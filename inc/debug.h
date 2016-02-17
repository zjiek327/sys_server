/**
* @file    	debug.h
* @brief   	The debug assistant.
* @details 	The debug assistant.
* @author  	(JT)
* @date    	2015/10/30
* @version 	1.0
*
* @Copyright (c) 2015
*/

#ifndef _DEBUG_H
#define _DEBUG_H

#include <stdarg.h>
#include <stdio.h>


#define __DEBUG__ 1
#if __DEBUG__

#define DEBUG(fmt,...)  do{printf("%s %d %s::",__FILE__, __LINE__, __func__); \
	printf(fmt,##__VA_ARGS__); printf("\n");}while(0)
#define SLEEP(n) sleep(0)
#else

#define DEBUG(fmt,...)
#define SLEEP(n)

#endif

#define LOG(fmt,...)  do{printf("[%d]%s::", __LINE__, __func__); \
	printf(fmt,##__VA_ARGS__); printf("\n");}while(0)

#define DIE(fmt,...) \
	do{\
		printf("%s %d %s::",__FILE__, __LINE__, __func__); \
		printf(fmt,##__VA_ARGS__); \
		printf("\n"); \
		exit(-1); \
	}while(0)

#define PDIE(fmt,...) \
	do{\
		printf("%s %d %s::",__FILE__, __LINE__, __func__); \
		printf(fmt,##__VA_ARGS__); \
		printf("\n"); \
		pthread_exit((void *)-1); \
	}while(0)



#endif

