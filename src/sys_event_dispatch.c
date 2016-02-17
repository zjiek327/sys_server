#include "inc/sys_event.h"
#include "inc/sys_net_event.h"
#include "inc/sys_bt_event.h"
#include "inc/sys_serial_event.h"


extern void event_trigger(mngtask *mtask);

void event_trigger(mngtask *mtask)
{
// bluetooth
#ifdef __BLUETOOTH__
	tpool_add_work(event_bluetooth, mtask);
#endif

// serail 
#ifdef __SERIAL__
	tpool_add_work(event_serial, mtask);
#endif

// ethernet
#ifdef __ETHERNET__
	tpool_add_work(event_ethernet, mtask);
#endif

	usleep(1000);
}

