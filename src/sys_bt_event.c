
#include "inc/sys_bt_event.h"



void *event_bluetooth(void *arg)
{
	//system_server *sys = (system_server *)arg;
	//bluetooth_manage *sys_bt = &sys->sTsysbt;
	mngtask *mtask = (mngtask *)arg;
	bluetooth_manage *sys_bt = mtask->bt;
	
	while(sys_bt->uCrunning){
		DEBUG("I am in event_bluetooth!^_^ ");
		sleep(10);
	}
}
