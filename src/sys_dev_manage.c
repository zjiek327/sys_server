/**
* @file    sys_dev_manage.c
* @brief   The api for system server.
* @details The api for system server.
*		   These APIs are provide function to operate device, likes
*		   register/unregister, change device permission, get device
*		   status, control turn on/off, etc.
* @author  (JT)
* @date    2015/10/26 
* @version 1.0
*
* @Copyright (c) 2015
*/

#include "inc/sys_dev.h"
#include "inc/sys_server.h"

static dev_basic *firstdev = NULL;

/**
 * sys_dev_register - Register device into devices list, it status will be
 *					   show in client screen if it register successful.
 * 	
 * @param[in]  devices The head of devices list.
 * @param[in]  device  This device will be inserted to devices list.
 *
 * @return 0:success    -1:fail
 */
int sys_dev_register(dev_manage *devices, dev_basic *device)
{
	if(!devices || !device)
		return -1;
	
	/* insert the node 'device' into devices list */
	pthread_mutex_lock(&devices->devmlock);
	list_add(&devices->sTdevice->sTdev_list, &device->sTdev_list);
	pthread_mutex_unlock(&devices->devmlock);

	return 0;
}

/**
 * sys_dev_unregister - Unregister device from devices list, it status will be
 *					   clean in client screen if it unregister successful.
 * 	
 * @param[in]  devices The head of devices list.
 * @param[in]  device  This device will be delete from devices list.
 *
 * @return 0:success    -1:fail
 */
int sys_dev_unregister(dev_manage *devices, dev_basic *device)
{
	if(!devices || !device)
		return -1;
	
	/* insert the node 'device' into devices list */
	pthread_mutex_lock(&devices->devmlock);
	//list_del(&devices->sTdevice->sTdev_list, &device->sTdev_list);
	pthread_mutex_unlock(&devices->devmlock);

	return 0;
}

/**
 * sys_dev_init - Init device member, include permission operation, attribute etc.
 * 	
 * @param[in]  devices  The head of devices list.
 * @param[in]  dev_attr The attribute of device.
 * @param[in]  type     The type of device.
 *
 * @return 0:success    -1:fail
 */
int sys_dev_init(dev_basic *device, void *dev_attr, char type)
{
	device->gCdevtype = type;
	device->gIdev_per_operation &= 0;
	device->gIdev_per_operation |= ELEXSMRT_DEV_UNREGISTERED;
	device->gIdev_per_operation |= ELEXSMRT_DEV_NONE;
	device->priv = dev_attr;
	//LIST_HEAD_INIT(&device->sTdev_list);
	if(pthread_mutex_init(&device->dev_basic_lock, NULL) < 0)
		return -1;	
	
	return 0;
}

/**
 * sys_get_dev_type - Get the type of device, and it will 
 *
 */
int sys_get_dev_type(dev_basic *device)
{
	return device->gCdevtype;
}

int system_net_init(net_manage *net_info)
{
	int ret = -1;

	net_info->uCrunning = 1;
	ret = obtain_netdev_info(net_info);
	if(ret < 0)
		exit(-1);
}

int system_dev_init(dev_manage *devm)
{
	int size = sizeof(dev_basic);

	firstdev = (dev_basic *)malloc(size);
	if(!firstdev)
		DIE("Init device manage fialed!");
	else{
		memset(firstdev, 0, sizeof(size));
		strcpy(firstdev->uCname, "first");
		strcpy(firstdev->uCserialno, "first");
		firstdev->gCdevtype = ELEXSMRT_FIRST;
		firstdev->gIdev_per_operation = ELEXSMRT_DEV_NONE;
		firstdev->priv = NULL;
		strcpy(firstdev->uCbuildname, "sys");
		strcpy(firstdev->uCusername, "sys");
		//first->dev_basic_lock = PTHREAD_MUTEX_INITIALIZER;
		if(pthread_mutex_init(&firstdev->dev_basic_lock, NULL) < 0)
			DIE("Init device manage fialed!");
		INIT_LIST_HEAD(&firstdev->sTdev_list);
		firstdev->next = firstdev;
	}
	devm->sTdevice = firstdev;
	devm->pfirst = firstdev;
	devm->plast = firstdev;

	return 0;
}

dev_basic *get_first_dev(void)
{
	return firstdev;
}



















