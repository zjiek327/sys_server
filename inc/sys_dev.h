#ifndef __ELEX_DEV_H__
#define __ELEX_DEV_H__

/**
* @file    sys_dev.h
* @brief   Device general define for system server.
* @details Device general define for system server.
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

#include "sys_smrt.h"
#include "vtype.h"
#include "dlist.h"

#define ELEXSMRT_SERIALNO_SIZE	128


/* devices type */
enum DEV_TYPE
{
	ELEXSMRT_FIRST = 0x0, ///< first flag
	ELEXSMRT_LIGHT, ///< lights
	ELEXSMRT_NEST, ///< temperature control
	ELEXSMRT_TV, ///< televisions
	ELEXSMRT_AIRCONDITION, ///< air-conditions
	ELEXSMRT_DOOR ///< doors
};

/*----------------------------------------------------------------*/
/*
 * DEV_OPERATE and DEV_PERMISSION are means to change a device
 * is not successful if not permission of system.
 * The high 24 bit is DEV_OPERATE and low 8 bit is DEV_PERMISSION.
 * If you want to get the permission to configure device, the 
 * condition is the value of bit ELEXSMRT_DEV_CONFIG and 
 * ELEXSMRT_DEV_NORMAL are both set as 1.
 */

/* all the operation of devices */
enum DEV_OPERATE
{
	ELEXSMRT_DEV_NONE=0x100,   ///< do nothing
	ELEXSMRT_DEV_TURNON, ///< device turn on
	ELEXSMRT_DEV_TURNOFF,///< device turn off
	ELEXSMRT_DEV_CONFIG, ///< configure device
	ELEXSMRT_DEV_REGISTER, ///< register a device
	ELEXSMRT_DEV_UNREGISTER, ///< remove a device
	NONE_

};
/* permission of devices */
enum DEV_PERMISSION
{
	ELEXSMRT_DEV_REGISTERED=0x1,   ///< device was registered
	ELEXSMRT_DEV_UNREGISTERED, ///< device is not register
	/*
	 * At this mode, meas that device will be
	 * protected by system, many of devices operation
	 * must be allowed by system.
	 */
	ELEXSMRT_DEV_PROTECTED,///< device will protected by system
	ELEXSMRT_DEV_NORMAL ///< configure device
};
/*---------------------------------------------------------------*/


/*---------------------------------------------------------------*/
/* light attribute */
struct elex_light
{
	unsigned char status; ///< 0:off  1:on
	/* its value range is 0~255*/
	unsigned char brightness; ///< the light brightness
};

/* nest attribute */
struct elex_nest
{
	unsigned char uCstatus; ///< 0:off  1:on
	/* its value range is -2^31~2^31-1*/
	double en_temperature; ///< the nest environment temperature
	double en_humidity; ///< the nest environment humidity
	unsigned char uCdevmac[ELEXSMRT_MAC_SIZE]; ///< device's mac address
	float dev_temperature; ///< Temperature of this device
	float room_temperature; ///< Temperature of this room
	float max_temperature; ///< max temperature of this device
	float min_temperature; ///< min temperature of this device
	float basic_temperature; ///< basic temperature of this device
};

/* tv attribute */
struct elex_tv
{
	unsigned char status; ///< 0:off  1:on
	/* its value range is 0~255*/
	unsigned char brightness; ///< the tv brightness
};
/*---------------------------------------------------------------*/

/* device operate */
typedef struct dev_ops
{
	/**
	 * @obj initial object which is the device's private attribute
	 * @type device type, more detail @DEV_TYPE
	 */
	int (*dev_init)(void *obj, unsigned char type);
	int (*dev_read)(void *obj, unsigned char type);
	int (*dev_set)(void *obj, unsigned char type);
}dev_ops;

/* devices general attribute */
typedef struct dev_basic
{
	unsigned char uCname[ELEXSMRT_NAME_SIZE];
	unsigned char uCserialno[ELEXSMRT_SERIALNO_SIZE];
	char gCdevtype;///< details see @@enum DEV_TYPE@@
	/* permission of operate device, for example:turn on/off and config,etc */
	uint32 gIdev_per_operation;///< details see @DEV_OPERATE and @DEV_PERMISSION
	dev_ops ops;
	
	void *priv; ///< device's private attribute, nest/light/doors/tv's attribute

	unsigned char uCusername[ELEXSMRT_USER_MAXNUM]; ///< Name of user who is the device owner
	unsigned char uCbuildname[ELEXSMRT_USER_MAXNUM]; ///< Name of build house whom is the device owner
	unsigned char uCusedrecord[ELEXSMRT_REQ_DATA_LENGTH]; ///< history of this device has used
	
	/* protecte device data when multi thread to change it*/
	pthread_mutex_t dev_basic_lock;
	
	/* kenerl link list manage all registered devices */
	struct list_head sTdev_list;
	struct dev_basic *next;
}dev_basic;

typedef struct dev_manage
{
	dev_basic *pfirst;
	dev_basic *plast;
	dev_basic *sTdevice;
	uint32 uIdev_count; ///< the count of all registered devices
	/* protecte device list when multi thread to change it*/
	pthread_mutex_t devmlock;	
	
}dev_manage;





/**
 * get_first_dev - Get the first device node from global device list.
 * 	
 * @param[in] null 
 *
 * @return the first device node.
 */
extern dev_basic *get_first_dev(void);













#endif
