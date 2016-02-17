
#include "inc/sys_serial_event.h"


/* initial device pt1000 serial */
void pt1000_serial_info_init(void *arg)
{
	serial_manage *serial = (serial_manage *)arg;
	//strcpy(serial->gCsname, "/dev/ttyUSB1");
	strcpy(serial->gCsname, "/dev/ttymxc4");
	serial->gIspeed = 9600;
	//serial->gIspeed = 115200;
	serial->gIbits = 8;
	serial->gCevent = 'N';
	serial->gIflowctrl = 0;
	serial->gIstop = 1;
}

int open_serial(char *devname)
{
	int fd = -1;

	fd = open(devname, O_RDWR|O_NOCTTY|O_NDELAY);
	if(fd < 0){
		printf("%s-", devname);
		perror("can't Open Serial Port");
		serial_errno = EDEVOPEN;
		return fd;
	}
	else if(fd > 0){   
		printf("open %s .....\n", devname);
	}
	if(fcntl(fd, F_SETFL, 0) < 0){
		serial_errno = errno;
		perror("fcntl failed\n");
		return(-1);
	}
	if(0 == isatty(STDIN_FILENO)){
		printf("standard input is not a terminal device\n");
		return(-1);
	}

	return fd;
}

#if 0
int set_serial(int fd, void *arg)
{
	serial_manage *serial = (serial_manage *)arg;
	struct termios newtio, oldtio;

	if(tcgetattr(fd, &oldtio)  !=  0){
		perror("SetupSerial 1");
		serial_errno = EDEVSET;
		return -1;
	}
	bzero(&newtio, sizeof(newtio));
	newtio.c_cflag |= CLOCAL | CREAD;
	newtio.c_cflag &= ~CSIZE;

	switch(serial->gIbits){
		case 7:
			newtio.c_cflag |= CS7;
			break;
		case 8:
			newtio.c_cflag |= CS8;
			break;
	}

	switch(serial->gCevent){
		case 'O':                     //奇校验
			newtio.c_cflag |= PARENB;
			newtio.c_cflag |= PARODD;
			newtio.c_iflag |= (INPCK | ISTRIP);
			break;
		case 'E':                     //偶校验
			newtio.c_iflag |= (INPCK | ISTRIP);
			newtio.c_cflag |= PARENB;
			newtio.c_cflag &= ~PARODD;
			break;
		case 'N':                    //无校验
			newtio.c_cflag &= ~PARENB;
			break;
	}
	switch(serial->gIspeed){
		case 2400:
			cfsetispeed(&newtio, B2400);
			cfsetospeed(&newtio, B2400);
			break;
		case 4800:
			cfsetispeed(&newtio, B4800);
			cfsetospeed(&newtio, B4800);
			break;
		case 9600:
			cfsetispeed(&newtio, B9600);
			cfsetospeed(&newtio, B9600);
			break;
		case 115200:
			cfsetispeed(&newtio, B115200);
			cfsetospeed(&newtio, B115200);
			break;
		default:
			cfsetispeed(&newtio, B9600);
			cfsetospeed(&newtio, B9600);
			break;
	}
	if(serial->gIstop == 1){
		newtio.c_cflag &= ~CSTOPB;
	}
	else if(serial->gIstop == 2){
		newtio.c_cflag |= CSTOPB;
	}
	newtio.c_cc[VTIME] = 0;
	newtio.c_cc[VMIN]  = 0;
	tcflush(fd, TCIFLUSH);
	if((tcsetattr(fd, TCSANOW, &newtio)) != 0){
		perror("com set error");
		serial_errno = EDEVSET;
		return -1;
	}
	DEBUG("set done!\n");

	return 0;
}
#endif

int set_serial(int fd, void *arg)
{ 
	serial_manage *serial = (serial_manage *)arg;
	int i; 
	struct termios options; 
	int speed_arr[] = {
		B38400, B115200, B19200, B9600, B4800, B2400, B1200, B300,
		B38400, B115200, B19200, B9600, B4800, B2400, B1200, B300 
	};
	int name_arr[] = {
		38400, 115200, 19200, 9600, 4800, 2400, 1200, 300,
		38400, 115200, 19200, 9600, 4800, 2400, 1200, 300 
	};  

	if(tcgetattr(fd, &options)  !=  0){  
		perror("SetupSerial 1");     
		return(-1);  
	}
	for(i= 0; i<sizeof(speed_arr)/sizeof(int); i++) {		
		if(serial->gIspeed == name_arr[i]){
			cfsetispeed(&options, speed_arr[i]);  
			cfsetospeed(&options, speed_arr[i]);   
		}
	}	 
	options.c_cflag |= CLOCAL;
	options.c_cflag |= CREAD;
	switch(serial->gIflowctrl){
		case 0 :
			options.c_cflag &= ~CRTSCTS;
			break;	
		case 1 :
			options.c_cflag |= CRTSCTS;
			break;
		case 2 :
			options.c_cflag |= IXON | IXOFF | IXANY;
			break;
	}

	options.c_cflag &= ~CSIZE; 
	switch(serial->gIbits){   
		case 5 :
			options.c_cflag |= CS5;
			break;
		case 6	:
			options.c_cflag |= CS6;
			break;
		case 7	:     
			options.c_cflag |= CS7; 
			break;
		case 8:     
			options.c_cflag |= CS8;
			break;   
		default:    
			fprintf(stderr,"Unsupported data size\n"); 
			return -1;
	}
	switch(serial->gCevent){   
		case 'n':
		case 'N': 
			options.c_cflag &= ~PARENB;  
			options.c_iflag &= ~INPCK;     
			break;  
		case 'o':   
		case 'O':    
			options.c_cflag |= (PARODD | PARENB);  
			options.c_iflag |= INPCK;              
			break;  
		case 'e':  
		case 'E':   
			options.c_cflag |= PARENB;        
			options.c_cflag &= ~PARODD;        
			options.c_iflag |= INPCK;       
			break;
		case 's': 
		case 'S': 
			options.c_cflag &= ~PARENB;
			options.c_cflag &= ~CSTOPB;
			break;  
		default:   
			fprintf(stderr,"Unsupported parity\n");    
			return -1; 
	}
	switch(serial->gIstop){
		case 1:    
			options.c_cflag &= ~CSTOPB;  
			break;  
		case 2:    
			options.c_cflag |= CSTOPB;  
			break;
		default:    
			fprintf(stderr,"Unsupported stop bits\n");  
			return -1; 
	} 

	options.c_oflag &= ~OPOST; 

	options.c_cc[VTIME] = 1;    
	options.c_cc[VMIN] = 1; 

	tcflush(fd, TCIFLUSH);

	if(tcsetattr(fd, TCSANOW, &options) != 0){ 
		perror("com set error!\n");   
		return -1;  
	} 
	return 0;  
}



int start_serial(void *arg)
{
	serial_manage *serial = (serial_manage *)arg;
	smrt_event_op *op = &(serial->op);

	serial->uCrunning = 1;
	if(op->evop_init && op->evop_open && op->evop_set){
		op->evop_init(serial);
		serial->gIhandle = op->evop_open(serial->gCsname);
		if(serial->gIhandle < 0){
			serial_errno = EDEVOPEN;
			return -1;
		}
		op->evop_set(serial->gIhandle, serial);
	}
	else{
		serial_errno = EDEVINIT;
		return -1;
	}

	return 0;
}

int system_serial_init(serial_manage *serial_info)
{
	int ret = -1;

	serial_info->op.evop_init = pt1000_serial_info_init;
	serial_info->op.evop_open = open_serial;
	serial_info->op.evop_set = set_serial;
	serial_info->op.evop_read = dev_read;
	serial_info->op.evop_write = dev_write;
	serial_info->op.evop_start = start_serial;
	serial_info->op.evop_stop = dev_stop;

	return 0;
}

void *serial_write(void *arg)
{
	serial_manage *serial = (serial_manage *)arg;
	smrt_event_op *op = &(serial->op);
	int len = -1;
	char com[11] = {'\0'};
	char snd[8] = {0x01, 0x04, 0x0, 0x0, 0x0, 0x01, 0x31, 0xca};
	//char snd[8] = {0x01, 0x04, 0x00, 0x01, 0x00, 0x01, 0x60, 0x0A};

	while(serial->uCrunning){
		printf("Please input any character:");
		fgets(com, 10, stdin);
		len = op->evop_write(serial->gIhandle, snd, 8);
		if(len <= 0){
			op->evop_stop(&serial->uCrunning);
		}
		printf("+++++++%x %x %x %x %x %x %x %x+++++++\n", snd[0], snd[1], snd[2], snd[3], snd[4], snd[5], snd[6], snd[7]);
	}

	DEBUG("Serial writer exit!");
}

/**
 * event_serial - Serial event trigger, this module get task from serial
 *				  interface(@intent) and insert into pending list when 
 *				  the data has generated.
 * 	
 * @param[in]  arg Resources of this module.
 *
 * @return value present condition at this module operation
 *
 */
void *event_serial(void *arg)
{
	mngtask *mtask = (mngtask *)arg;
	serial_manage *serial = mtask->serial;
	smrt_event_op *op = &(serial->op);
	char data[DATA_SIZE] = {'\0'};
	int len = -1;
	int ret = -1;
	int i;
	pending *pendnode = NULL;

	DEBUG("I am in event_serial! %d ^_^ ", serial->gIhandle);
	printf("serial_errno:%d\n", serial_errno);
	ret = op->evop_start(serial);
	if(ret < 0)
		//PDIE("|ERROR| Start <%s>", serial->gCsname);
		op->evop_stop(&serial->uCrunning);
	
	tpool_add_work(serial_write, serial);
	while(serial->uCrunning){
		memset(data, 0, DATA_SIZE);
		len = op->evop_read(serial->gIhandle, data, 100);
		if(len > 0){
			pendnode = (pending *)malloc(sizeof(pending));
			if(pendnode){
				mtask->pend.action = 't';
				memcpy(pendnode->command, data, 100);
				//printf("-------%x %x %x %x %x %x %x %x %x-------\n", pendnode->command[0], pendnode->command[1], pendnode->command[2], pendnode->command[3], pendnode->command[4], pendnode->command[5], pendnode->command[6], pendnode->command[7], pendnode->command[8]);
				task_pending_add(&mtask->pend, pendnode);
				mtask->trigger(mtask->intent, mtask->pend.action);
			}
			else{
				LOG("Trigger:allocate pending node failed.");
				continue;
			}
		}
		else{
			op->evop_stop(&serial->uCrunning);
		}
	}

	DEBUG("Serial reader exit!");
}


