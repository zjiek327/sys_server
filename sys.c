/**
 * @file    	sys.c
 * @brief   	The api for system server.
 * @details 	The api for system server.
 *		   	This is main function entrance at system server program.
 * @author  	(JT)
 * @date    	2015/10/26
 * @version 	1.0
 *
 * @Copyright (c) 2015
 */
#include "inc/sys_server.h"
#include "inc/sys_task.h"

int running = 1;
system_server sys_server;
mngtask *gtask = NULL;

void *event_entry(void *arg)
{

}

void do_system_server(void)
{
	int ret;

	if(tpool_create(ELEXSMRT_TPOOL_MAXNUM) != 0)
		exit(1);

	//event_trigger(&sys_server);
	event_trigger(gtask);

	while(running){
		sleep(20);
		DEBUG("System server is running......");
		//tpool_add_work(event_entry, void * arg);
	}

	tpool_destroy();
}

int system_info_init(system_info *sys_info)
{
	int ret = -1;
	struct utsname un;

	ret = uname(&un);
	if(ret < 0){
		perror("get system info");	
		exit(1);
	}

	strcpy(sys_info->gChostname, un.nodename);
	strcpy(sys_info->gCsys_platform, un.sysname);
	strcpy(sys_info->gCrelease, un.release);
	strcpy(sys_info->gCversion, un.version);
	strcpy(sys_info->gCmachine, un.machine);

#if __DEBUG__
	printf("The system info:\n");
	printf("\tSystem name:%s\n", sys_info->gCsys_platform);
	printf("\tHost name:%s\n", sys_info->gChostname);
	printf("\tSystem release:%s\n", sys_info->gCrelease);
	printf("\tSystem version:%s\n", sys_info->gCversion);
	printf("\tMachine:%s\n", sys_info->gCmachine);

#endif
}

int system_bt_init(bluetooth_manage *bt_info)
{
	bt_info->uCrunning = 1;
}

int obtain_netdev_info(net_manage *net_info)
{
	int fd;
	int idx = 0;
	int interface_num;
	struct ifreq buf[16];
	struct ifconf ifc;
	struct ifreq ifrcopy;

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0){
		perror("socket");
		close(fd);
		return -1;
	}

	ifc.ifc_len = sizeof(buf);
	ifc.ifc_buf = (caddr_t)buf;
	if (!ioctl(fd, SIOCGIFCONF, (char *)&ifc))
	{
		interface_num = ifc.ifc_len / sizeof(struct ifreq);
		DEBUG("There are %d interface!", interface_num);
		while (interface_num-- > 0){
			if(0 == strcmp(buf[interface_num].ifr_name, "lo"))
				continue;
			strcpy(net_info[idx].sTnet.gCintername, buf[interface_num].ifr_name);
			printf("\ndevice name: %s\n", net_info[idx].sTnet.gCintername);

			//ignore the interface that not up or not runing
			ifrcopy = buf[interface_num];
			if (ioctl(fd, SIOCGIFFLAGS, &ifrcopy)){
				DEBUG("ioctl: %s", strerror(errno));
				close(fd);
				return -1;
			}

			//get the mac of this interface
			if (!ioctl(fd, SIOCGIFHWADDR, (char *)(&buf[interface_num]))){
				snprintf(net_info[idx].sTnet.gCmac, 18, "%02x:%02x:%02x:%02x:%02x:%02x",
						(unsigned char)buf[interface_num].ifr_hwaddr.sa_data[0],
						(unsigned char)buf[interface_num].ifr_hwaddr.sa_data[1],
						(unsigned char)buf[interface_num].ifr_hwaddr.sa_data[2],
						(unsigned char)buf[interface_num].ifr_hwaddr.sa_data[3],
						(unsigned char)buf[interface_num].ifr_hwaddr.sa_data[4],
						(unsigned char)buf[interface_num].ifr_hwaddr.sa_data[5]);
				printf("device mac: %s\n", net_info[idx].sTnet.gCmac);
			}
			else{
				DEBUG("ioctl: %s", strerror(errno));
				close(fd);
				return -1;
			}

			//get the IP of this interface
			if (!ioctl(fd, SIOCGIFADDR, (char *)&buf[interface_num])){
				snprintf(net_info[idx].sTnet.gCipaddr, sizeof(net_info[idx].sTnet.gCipaddr), "%s",
						(char *)inet_ntoa(((struct sockaddr_in *)&(buf[interface_num].ifr_addr))->sin_addr));
				printf("device ip: %s\n", net_info[idx].sTnet.gCipaddr);
			}
			else{
				DEBUG("ioctl: %s", strerror(errno));
				close(fd);
				return -1;
			}

			//get the broad address of this interface
			if (!ioctl(fd, SIOCGIFBRDADDR, &buf[interface_num])){
				snprintf(net_info[idx].sTnet.gCbcastaddr, sizeof(net_info[idx].sTnet.gCbcastaddr), "%s",
						(char *)inet_ntoa(((struct sockaddr_in *)&(buf[interface_num].ifr_broadaddr))->sin_addr));
				printf("device bcastaddr: %s\n", net_info[idx].sTnet.gCbcastaddr);
			}
			else{
				DEBUG("ioctl: %s", strerror(errno));
				close(fd);
				return -1;
			}

			//get the subnet mask of this interface
			if (!ioctl(fd, SIOCGIFNETMASK, &buf[interface_num])){
				snprintf(net_info[idx].sTnet.gCnetmask, sizeof(net_info[idx].sTnet.gCnetmask), "%s",
						(char *)inet_ntoa(((struct sockaddr_in *)&(buf[interface_num].ifr_netmask))->sin_addr));
				printf("device subnetMask: %s\n", net_info[idx].sTnet.gCnetmask);
			}
			else{
				DEBUG("ioctl: %s", strerror(errno));
				close(fd);
				return -1;
			}
			idx++;
		}
	}
	else{
		DEBUG("ioctl: %s", strerror(errno));
		close(fd);
		return -1;
	}

	return 0;
}

int main(int argc, char *argv[])
{
	system_info_init(&sys_server.sTsysinfo);
	system_dev_init(&sys_server.sTsysdev);

#ifdef __ETHERNET__
	system_net_init(sys_server.sTsysnet);
	printf("\ndevice name: %s\n", sys_server.sTsysnet[0].sTnet.gCintername);
	printf("device mac: %s\n", sys_server.sTsysnet[0].sTnet.gCmac);
	printf("device ip: %s\n", sys_server.sTsysnet[0].sTnet.gCipaddr);
	printf("device bcastaddr: %s\n", sys_server.sTsysnet[0].sTnet.gCbcastaddr);
	printf("device subnetMask: %s\n", sys_server.sTsysnet[0].sTnet.gCnetmask);
#endif
#ifdef __BLUETOOTH__
	system_bt_init(&sys_server.sTsysbt);
#endif
#ifdef __SERIAL__
	system_serial_init(&sys_server.sTsys_serial);
#endif

	gtask = task_init(&sys_server);

	do_system_server();

	return 0;
}

