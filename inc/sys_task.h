#ifndef __ELEX_TASK_H__
#define __ELEX_TASK_H__

#include "dlist.h"
#include "vtype.h"
#include "sys_server.h"

/*
 * ##根据协议划分任务类型##
 *
 * 如果有多个项目，每个任务有自己的一套协议，在处理
 * 任务请求时只需将相同协议的任务归类，就可以区别不
 * 同任务的处理方式，分配相应的资源。
 *
 * 举例：
 *	判断type=0x200001任务属于哪种项目什么设备触发
 *	switch(type & TASK_ITEM_TYPE){ // 0x200000
 *		case ELEXSMRT_NET_TEMPERATURE:
 *			do_something();
 *			break;
 *		case ELEXSMRT_ELECTRIC_HEATING:
 *			do_something();
 *			break;
 *		case ELEXSMRT_WATER_HEATING:
 *			do_something();
 *			break;
 *	}
 *
 *	switch(type & TASK_DEV_TYPE){ // 0x01
 *		case ELEXSMRT_TEMPERATURE:
 *			do_something();
 *			break;
 *		case ELEXSMRT_LIGHT:
 *			do_something();
 *			break;
 *		case ELEXSMRT_LOCK:
 *			do_something();
 *			break;
 *	}
 */
/* high 12bit present what project is.  low 20bit present what device is. */
#define TASK_ITEM_TYPE  0xfff00000
#define TASK_DEV_TYPE 0x000fffff

/* for @TASK_DEV_TYPE  part device */
#if 0
enum TASK_DTYPE
{
	ELEXSMRT_TEMPERATURE = 0x1, ///< device：温控器
	ELEXSMRT_LIGHT, ///< device：灯
	ELEXSMRT_LOCK, ///< device：锁
	ELEXSMRT_
};
#endif

/* for @TASK_ITEM_TYPE part protocol */
enum TASK_ITYPE
{
	ELEXSMRT_NET_TEMPERATURE = 0x100000, ///< 互联网温控项目
	ELEXSMRT_ELECTRIC_HEATING, ///< 电暖项目
	ELEXSMRT_WATER_HEATING, ///< 水暖项目
};

enum TASK_STATUS
{
	TASK_NONE = 0x0,
	PENDING = 0x1,
	START,
	RUNNING,
	UNEXCUTE,
	END
};

/*-----------------------生产者-消费者节点-----------------------*/
typedef struct pending
{
	char action; ///< operation type   't':task  'p': task prarity
	char command[DATA_SIZE];

	struct list_head pendlist;
}pending;
/*-----------------------生产者-消费者节点-----------------------*/

/* 
 * Task node object
 *
 * @type      - High 12bit present which project this task is.
 *		        Low 20bit present whats kind of this task is.
 * @status    - 
 * @taskcom   - The pending node which will be parsed.
 * @dev_latest- The device latest infomation after it has been
 *				operated.
 * @dev_currt - The device current infomation and it will be
 *				update by @dev_latest if this task operate successful.
 *
 */
typedef struct taskl
{
	uint32 type; ///< task type, details:@TASK_ITEM_TYPE and @DEV_TYPE(sys_dev.h)
	unsigned char status; ///< task status @TASK_STATUS
	pending *taskcom;

	/* 
	 * 成功执行完操作设备的任务后，更新当前设备状态 
	 * memcpy(dev_currt, dev_latest);
	 */
	dev_basic *dev_latest;
	dev_basic *dev_currt;

	struct list_head tasklist;
}taskl;

/*
 * Task manage object, contents pending list and schedule list, 
 * fifo file description which use to communicate with allocation
 * thread.
 * Allocation thread get pending node, allocate resources to this
 * task, then set it state as 'start' and insert into schedule list.
 * Schedule thread get task node and excute this task.
 */
typedef struct mngtask
{
	pthread_t allocateid;
	pthread_t scheduleid;
	int intent; ///< 任务通知接口（有名管道）
	char infac_fifo_name[ELEXSMRT_NAME_SIZE]; ///< 通知接口管道名

	pending pend;

	serial_manage *serial;
	net_manage *net;
	bluetooth_manage *bt;

	taskl *schtask;

	void (*trigger)(int, char);
	void (*triggered)(int, char *);
}mngtask;


extern taskl *task_parse(mngtask *);
extern void *task_allocte_resources(void *);
extern void *task_allocate(void *);
extern void task_triggered(int , char *);
extern void task_trigger(int , char);
extern int inface_fifo_init(const char *);
extern void task_pending_init(pending *);
extern taskl *task_sched_list_init(void);
extern mngtask *task_init(system_server *);

extern void *task_schedule(void *);
extern void task_excute(taskl *);
extern void task_schedule_add(mngtask *, taskl *);
extern taskl *task_schedule_acquire(taskl *);
extern pending *task_pending_acquire(pending *);
extern void task_pending_add(pending *, pending *);

#endif
