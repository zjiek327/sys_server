
#ifndef __ELEX_ENTYPE_H__
#define __ELEX_ENTYPE_H__

/**
* @file    sys_entype.h
* @brief   The command type for system server.
* @details The command type for system server.
*		   All command type defined here, we use enum type to manage,
*		   you can addition and delete a operation type untrammeled.
*		   System could known what to do by these types when client 
*		   sent various different kinds of request.
* @author  (JT)
* @date    2015/10/26
* @version 1.0
*
* @Copyright (c) 2015
*/

enum COM_TYPE
{
	/* operate device */
	ELEXCOM_GET_DEVICE_BASIC_INFO, ///< get basic infomation of device
	ELEXCOM_SET_DEVICE_BASIC_INFO,///< set basic infomation of device
	ELEXCOM_OPERATE_DEVICE_REGISTER,///< register device into devices list
	ELEXCOM_OPERATE_DEVICE_UNREGISTER,///< remove device from devices list
	ELEXCOM_OPERATE_DEVICE_TURNON,///< turn on
	ELEXCOM_OPERATE_DEVICE_TURNOFF,///< turn off
	ELEXCOM_OPERATE_DEVICE_CONFIG,///< configure device arguments
	/**
	 * There have many different device, so the operation of them are also
	 * difference. fix me...
	 */
	NONE__
};













#endif
