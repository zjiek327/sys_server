#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>

#include <event.h>


#define PORT        25341
#define BACKLOG     5
#define MEM_SIZE    1024

struct event_base* nbase;
struct event_base* base;
struct sock_ev {
	struct event* read_ev;
	struct event* write_ev;
	struct event* timeout_ev;
	char* buffer;
	struct timeval tv;
	int newfd;
};

void release_sock_event(struct sock_ev* ev)
{
	printf("release sock event!\n");
	event_del(ev->read_ev);
	free(ev->read_ev);
	free(ev->write_ev);
	//free(ev->buffer);
	free(ev);
}

void on_write(int sock, short event, void* arg)
{
	int len = 0;
	char* buffer = (char*)arg;
	len = send(sock, buffer, strlen(buffer), 0);
	printf("send data:%s, size:%d\n", buffer, len);

	free(buffer);
}

void on_read(int sock, short event, void* arg)
{
	struct event* write_ev;
	int size;
	struct sock_ev* ev = (struct sock_ev*)arg;
	ev->buffer = (char*)malloc(MEM_SIZE);
	bzero(ev->buffer, MEM_SIZE);
	size = recv(sock, ev->buffer, MEM_SIZE, 0);
	printf("receive data:%s, size:%d\n", ev->buffer, size);
	if (size == 0) {
		release_sock_event(ev);
		close(sock);
		return;
	}
	event_set(ev->write_ev, sock, EV_WRITE, on_write, ev->buffer);
	event_base_set(nbase, ev->write_ev);
	event_add(ev->write_ev, NULL);

//	evutil_timerclear(&ev->tv);
//	ev->tv.tv_sec = 5;
//	event_add(ev->timeout_ev, &ev->tv);	
}

int lasttime;
void timeout_cb(int sock, short event, void *arg)
{
	struct sock_ev* ev = (struct sock_ev*)arg;
	int newtime = time(NULL);

	printf("%s: called at %d: %d\n", __func__, newtime,
			newtime - lasttime);
	lasttime = newtime;

	printf("rest of timeout:%d\n", ev->tv.tv_sec);
	evutil_timerclear(&ev->tv);
	ev->tv.tv_sec = 3;
	event_add(ev->timeout_ev, &ev->tv);	
}

int efd[4];
#define ERR -1
#define SUCC 0

int stm_setfd_noblocking(int fd)
{
	int flag = -1;
	
	flag = fcntl(fd, F_GETFL, 0);
	if(flag < 0)
		return flag;
	flag = fcntl(fd, F_SETFL, flag|O_NONBLOCK);
	if(flag < 0)
		return flag;

	return 0;
}

int stm_epoll_add_inevent(int idx, int fd)
{
	struct epoll_event event;

	event.data.fd = fd;
	event.events = EPOLLIN;
	if (epoll_ctl(efd[idx], EPOLL_CTL_ADD, fd, &event) < 0){
		perror ("epoll_ctl");
		return ERR;
	}

	if(stm_setfd_noblocking(fd) < 0)
		return ERR;

	return SUCC;
}

int stm_epoll_init_and_addevent(int idx, int fd)
{
	struct epoll_event event;

	efd[idx] = epoll_create(16);
	if (efd[idx] == -1){
		perror ("epoll_create");
		return ERR;
	}

	if(stm_epoll_add_inevent(idx, fd) < 0)
		return ERR;

	return SUCC;
}

void *listen_thread(void *arg)
{
	int i, n;
	char buf[128] = {0};
	struct sock_ev* ev = (struct sock_ev*)arg;
	struct epoll_event events[16];

	stm_epoll_init_and_addevent(0, ev->newfd);

while(1){
	n = epoll_wait(efd[0], events, 16, 5000);
	if(n < 0 && errno != EINTR){
		perror("epoll_wait");
		pthread_exit(0);
	}
	else if(0 == n){
		printf("timeout in listen thread!\n");	
		close(ev->newfd);
		release_sock_event(ev);
		nbase->event_break = 1;
		break;
	}
	
#if 0
	for(i=0; i<n; i++){
		if((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || !(events[i].events & EPOLLIN))
			perror("epoll event");
		if(events[i].events & EPOLLIN){
			read(events[i].data.fd, buf, 128);
			printf("Listen thread get read event<fd:%d>!  receive:%s\n", events[i].data.fd, buf);
		}
	}
#endif
}
	printf("exit listen thread!\n");
	pthread_exit(0);
}

void *read_thread(void *arg)
{
	struct sock_ev* ev = (struct sock_ev*)arg;

	nbase = event_base_new();
#if 1
	ev->read_ev = (struct event*)malloc(sizeof(struct event));
	ev->write_ev = (struct event*)malloc(sizeof(struct event));
	ev->timeout_ev = (struct event*)malloc(sizeof(struct event));
	event_set(ev->read_ev, ev->newfd, EV_READ|EV_PERSIST, on_read, ev);
	event_base_set(nbase, ev->read_ev);
	event_add(ev->read_ev, NULL);

	//evutil_timerclear(&ev->tv);
	//ev->tv.tv_sec = 5;
	//event_base_set(base, ev->timeout_ev);
	//event_add(ev->timeout_ev, &ev->tv);

	//evtimer_set(ev->timeout_ev, timeout_cb, ev);
	//evutil_timerclear(&ev->tv);
	//ev->tv.tv_sec = 3;
	//event_base_set(nbase, ev->timeout_ev);
	//event_add(ev->timeout_ev, &ev->tv);
	//int lasttime = time(NULL);

	event_base_dispatch(nbase);
#endif

	printf("exit read thread!\n");
	pthread_exit(0);
}

void on_accept(int sock, short event, void* arg)
{
	pthread_t tid;
	struct sockaddr_in cli_addr;
	int sin_size;
	struct sock_ev* ev = (struct sock_ev*)malloc(sizeof(struct sock_ev));

	sin_size = sizeof(struct sockaddr_in);
	ev->newfd = accept(sock, (struct sockaddr*)&cli_addr, &sin_size);
	printf("get a client connect!\n");

	pthread_create(&tid, NULL, read_thread, ev);
	pthread_create(&tid, NULL, listen_thread, ev);
}

int main(int argc, char* argv[])
{
	struct sockaddr_in my_addr;
	int sock;

	struct sock_ev* ev = (struct sock_ev*)malloc(sizeof(struct sock_ev));
	ev->timeout_ev = (struct event*)malloc(sizeof(struct event));
	sock = socket(AF_INET, SOCK_STREAM, 0);
	int yes = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
	memset(&my_addr, 0, sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(PORT);
	my_addr.sin_addr.s_addr = INADDR_ANY;
	bind(sock, (struct sockaddr*)&my_addr, sizeof(struct sockaddr));
	listen(sock, BACKLOG);

	struct event listen_ev;
	base = event_base_new();
	event_set(&listen_ev, sock, EV_READ|EV_PERSIST, on_accept, NULL);
	event_base_set(base, &listen_ev);
	event_add(&listen_ev, NULL);

#if 0
	evtimer_set(ev->timeout_ev, timeout_cb, ev);
	evutil_timerclear(&ev->tv);
	ev->tv.tv_sec = 5;
	event_base_set(base, ev->timeout_ev);
	event_add(ev->timeout_ev, &ev->tv);
	int lasttime = time(NULL);
#endif

	event_base_dispatch(base);

	return 0;
}
