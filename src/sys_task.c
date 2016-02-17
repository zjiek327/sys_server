

#include "inc/sys_task.h"


static pthread_mutex_t pendlock;
static pthread_mutex_t taskschlock;
static sem_t tasksem_empty;
static sem_t tasksem_full;

/*
 * task_trigger - This function is a trigger productor.
 *
 * param[in] trig Trigger handle.
 * param[in] type Trigger type.
 *
 */
void task_trigger(int trig, char type)
{
	int ret = -1;

	do{
		ret = write(trig, &type, 1);
	}while(ret < 0 && errno == EINTR);
	if(ret < 0)
		perror("trigger");
}

/*
 * task_triggered - This function is a trigger receiver.
 *
 * param[in] trig Trigger handle.
 * param[in] type Trigger type.
 *
 */
void task_triggered(int trig, char *type)
{
	int ret = -1;

	do{
		ret = read(trig, type, 127);
	}while(ret < 0 && errno == EINTR);
	if(ret < 0)
		perror("triggered");
	LOG("<%s>", type);
}

/*
 * inface_fifo_init - Initial fifo, its a interface for task trigger.
 *
 * param[in] fifo FIFO name.
 *
 * return fd.
 */
int inface_fifo_init(const char *fifo)
{
	int fd = -1;
	int ret = -1;

	/* create fifo use @fifo */
	ret = mkfifo(fifo, 0666);
	if(ret < 0 && errno != EEXIST){
		perror("mkfifo");
		return ret;
	}
	
	fd = open(fifo, O_NOCTTY|O_RDWR);
	if(fd < 0){
		perror("open fifo");
		return fd;
	}
	
	return fd;
}

/* task_pending_init - Initial pending task list. */
void task_pending_init(pending *pend)
{
	memset(pend, 0, sizeof(pending));
	INIT_LIST_HEAD(&pend->pendlist);
}

/* task_sched_list_init - Initial schedule task list. */
taskl *task_sched_list_init(void)
{
	int ret = -1;
	taskl *schtask = NULL;

	schtask = (taskl *)malloc(sizeof(taskl));
	if(!schtask)
		return NULL;
	
	memset(schtask, 0, sizeof(taskl));
	INIT_LIST_HEAD(&schtask->tasklist);

	return schtask;
}

/* task_init - Initial task manage object. */
mngtask *task_init(system_server *sys)
{
	mngtask *mtask = NULL;
	int size = sizeof(mngtask);

	mtask = (mngtask *)malloc(size);
	if(!mtask)
		DIE("Init device manage fialed!");

	/* initial fifo interface */
	strcpy(mtask->infac_fifo_name, ELEXSMRT_INFACE_FIFONAME);
	mtask->intent = inface_fifo_init(mtask->infac_fifo_name);
	if(mtask->intent < 0){
		FREE(mtask);
		DIE("Init device manage fialed!");
	}

	/* get net and serial info from system server */
	mtask->net = &sys->sTsysnet[0];
	mtask->serial = &sys->sTsys_serial;
	mtask->bt = &sys->sTsys_bt;

	mtask->trigger = task_trigger;
	mtask->triggered = task_triggered;

	/* initial pending list of task */
	task_pending_init(&mtask->pend);

	/* initial schedule list of task */
	if(pthread_mutex_init(&taskschlock, NULL) < 0)
		DIE("Init device manage fialed!");
	mtask->schtask = task_sched_list_init();
	if(!mtask->schtask)
		DIE("Init device manage fialed!");
	
	if(pthread_mutex_init(&pendlock, NULL) < 0)
		DIE("Init device manage fialed!");
	sem_init(&tasksem_empty, 0, ELEXSMRT_TASK_MAXNUM);
	sem_init(&tasksem_full, 0, 0);

	if(pthread_create(&mtask->allocateid, NULL, task_allocate, mtask) < 0)
		DIE("Init device manage fialed!");
	if(pthread_create(&mtask->scheduleid, NULL, task_schedule, mtask) < 0)
		DIE("Init device manage fialed!");

	return mtask;
}

/*
 * task_pending_add - Insert a pend node to pending list.
 */
void task_pending_add(pending *pend, pending *pendnew)
{
	pthread_mutex_lock(&pendlock);
	list_add_tail(&pendnew->pendlist, &pend->pendlist);
	pthread_mutex_unlock(&pendlock);
}

/*
 * task_pending_acquire - Get a pend node from pending list.
 */
pending *task_pending_acquire(pending *pend)
{
	pending *pendfirst = NULL;

	pthread_mutex_lock(&pendlock);
	if(list_empty(&pend->pendlist))
		return NULL;

	pendfirst = list_first_entry(&pend->pendlist, pending, pendlist);
	if(!pendfirst)
		return NULL;

	list_del(&pendfirst->pendlist);
	pthread_mutex_unlock(&pendlock);

	return pendfirst;
}

/*
 * task_schedule_acquire - Get a task node from schedule list.
 */
taskl *task_schedule_acquire(taskl *tasksch)
{
	taskl *taskfirst = NULL;

	sem_wait(&tasksem_full);
	pthread_mutex_lock(&taskschlock);

	if(list_empty(&tasksch->tasklist)){
		pthread_mutex_unlock(&taskschlock);
		return NULL;
	}

	taskfirst = list_first_entry(&tasksch->tasklist, taskl, tasklist);
	if(!taskfirst){
		pthread_mutex_unlock(&taskschlock);
		return NULL;
	}

	list_del(&taskfirst->tasklist);

	pthread_mutex_unlock(&taskschlock);
	sem_post(&tasksem_empty);

	return taskfirst;
}

taskl *task_parse(mngtask *mtask)
{
	taskl *tasknode = NULL;

	tasknode = (taskl *)malloc(sizeof(taskl));
	if(!tasknode){
		LOG("Task:allocate for task node failed.");
		return NULL;
	}

	tasknode->taskcom = task_pending_acquire(&mtask->pend);
	if(!tasknode->taskcom){
		FREE(tasknode);
		LOG("Task:acquire pending task node failed.");
		return NULL;
	}

	return tasknode;
}

/*
 * task_schedule_add - Insert a task node to schedule list.
 */
void task_schedule_add(mngtask *mtask, taskl *tasknode)
{
	sem_wait(&tasksem_empty);
	pthread_mutex_lock(&taskschlock);

	list_add_tail(&tasknode->tasklist, &mtask->schtask->tasklist);

	pthread_mutex_unlock(&taskschlock);
	sem_post(&tasksem_full);
}

/*
 */
void *task_allocte_resources(void *arg)
{
	mngtask *mtask = (mngtask *)arg;
	taskl *tasknode = NULL;

	tasknode = task_parse(mtask);
	if(!tasknode)
		return ((void *)-1);

	task_schedule_add(mtask, tasknode);

	return ((void *)0);
}

void task_node_recycle(taskl *tasknode)
{
	FREE(tasknode->taskcom);
	FREE(tasknode->dev_latest);
	tasknode->dev_currt = NULL;
	tasknode->status = TASK_NONE;
	tasknode->type = ELEXSMRT_FIRST;
	FREE(tasknode);
}

void task_excute(taskl *tasknode)
{
	int i = 0;
	char *task = NULL;

	if(tasknode->taskcom){
		LOG("new task-----------------------|");
		task = tasknode->taskcom->command;
		for(i; i<8; i++){
			printf("%x ", task[i]);
		}
		printf("\n");
		LOG("new task-----------------------|");
	}

	task_node_recycle(tasknode);
}

/*
 * task_allocate - Allocate resources.
 *				   1.wait triggered
 *				   2.allocate resources to task node
 * param[in] arg task manage object
 */
void *task_allocate(void *arg)
{
	mngtask *mtask = (mngtask *)arg;
	char types[128] = {'\0'};

	while(running){
		memset(types, 0, 128);
		mtask->triggered(mtask->intent, types);

		if(list_empty(&mtask->pend.pendlist))
			continue;
		tpool_add_work(task_allocte_resources, mtask);
	}
}

/*
 * task_schedule - Schedule task.
 *				   1.get task node
 *				   2.excute task
 * param[in] arg task manage object
 */
void *task_schedule(void *arg)
{
	mngtask *mtask = (mngtask *)arg;
	taskl *tasknode = NULL;
	
	while(running){
		tasknode = task_schedule_acquire(mtask->schtask);
		if(!tasknode){
			LOG("Acquire task failed!");
			continue;
		}
		task_excute(tasknode);
	}
}







