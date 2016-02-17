#include "inc/sys_net_event.h"

char *filedata = NULL;
char *filename = "./www/login/Wopop.html";


/* 涉及到具体数据部分解析 */
void *event_deal(void *arg)
{
	DEBUG("In event deal, parse data");
}

int create_sock_listen(net_dev *netdev)
{
	int ret = -1;
	int sfd = -1;
	int opt = SO_REUSEADDR; 
	struct sockaddr_in serveraddr = {0};

	sfd = socket(AF_INET,SOCK_STREAM,0);
	if (sfd < 0){
		perror("create socket error");
		return sfd;
	} 
	setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR,(char *)&opt,sizeof(opt));

	serveraddr.sin_family = AF_INET;
	inet_aton(netdev->gCipaddr , &(serveraddr.sin_addr));
	serveraddr.sin_port = htons(ELEXSMRT_LISTEN_PORT);
	ret = bind(sfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
	if (ret < 0){
		perror("bind error");
		close(sfd);
		return ret;
	}

	ret = listen(sfd,1000);
	if (ret < 0){
		perror("listen error");
		close(sfd);
		return ret;
	}

	return sfd;
}

/* 网络时间监听处理 */
net_manage *event_ethernet_listening(net_manage *net_mg, char *eth)
{
	int idx;

	if(!net_mg)
		return NULL;

	for(idx=0; idx<ELEXSMRT_NETCARD_MAXNUM; idx++){
		if(0 == strcmp(net_mg[idx].sTnet.gCintername, eth)){
			break;
		}
	}

	net_mg[idx].listen = create_sock_listen(&net_mg[idx].sTnet);
	DEBUG("Create listen port successful, and the fd is %d !", net_mg[idx].listen);

	return &net_mg[idx];
}

/* 网络事件任务处理 */
int event_ethernet_deal(system_server *sys)
{
	tpool_add_work(event_deal, sys);
	DEBUG("Add @event_deal to parse data from client sent!^_^ \n");
}

void event_ethernet_timeout(int fd, short event, void *arg)
{
	/* connect server to get device data */
}

void event_ethernet_accepting(int fd, short event, void *arg)
{
#if 0
	rq_queue *rq_node;
	int newfd = -1;

	/* space for @rq_node*/
	rq_node = (rq_queue *)malloc(sizeof(rq_queue));
	if(!rq_node){
		DEBUG("malloc for @rq_node failded!");
		return;
	}

	/* excute accept() and obtian a new fd */
	rq_node->gIsock = accept(fd, );

	/* insert @rq_node to rq_queue */
	ev_push_rqqueue(rq_queue *hrq, rq_queue *new);
	/* note @@task_process@@ thread to process */
	task_add(ev_task_process);
#endif
}


/*---------------------------------------------------------------------------------------------------------*/
/* http */
//void read_file(char *uri)
void read_file(char *filename)
{
	int ret = -1;
	unsigned long size = 0;
	char *data;
	struct stat buf;

	if(stat(filename,&buf)<0)
	{
		printf("Read file error! \n");
		return;
	}


		FILE *f = fopen(filename, "rb");
		if (f == NULL)
		{
			fprintf(stderr,"Couldn't open file\n");
			return;
		}

		size = buf.st_size;
		filedata = (char *)malloc(size+1);
		memset(filedata,0,size+1);
		ret = fread(filedata, sizeof(char), size, f);
		fclose(f);

		fprintf(stderr," (%d bytes)\n",size);
}


void evhttp_request_handler(struct evhttp_request *req, void *arg)
{
	//初始化返回客户端的数据缓存
	char name[256] = "www/index.htm";
	char tmp[256] = {'\0'};
	struct evbuffer *buf;

	buf = evbuffer_new();


	/* 分析URL参数 */
	//char *name = strdup((char*) evhttp_request_uri(req));
	char *decode_uri = strdup((char*) evhttp_request_uri(req));
	//strcpy(decode_uri+5, name);
	if(strcmp(decode_uri, "/") == 0)
		strcpy(decode_uri, name);
	else{
		strcpy(tmp, decode_uri);
		strcpy(decode_uri, "www/login");
		strcat(decode_uri, tmp);
	}
	printf("Requested:%s\n", decode_uri);
	struct evkeyvalq http_query;

	evhttp_parse_query(decode_uri, &http_query);

	//接收GET表单参数name
	const char *http_input_name = evhttp_find_header(&http_query, "i");
	if(!http_input_name){
		//evbuffer_add_printf(buf, "Thanks for the request, but there is some error!");
		//evhttp_send_reply(req, HTTP_OK, "OK", buf);
		//return;
	}
	printf("http_input_name:%s\n", http_input_name);

	//处理输出header头
	evhttp_add_header(req->output_headers, "Content-Type", "text/html");
	evhttp_add_header(req->output_headers, "Connection", "keep-alive");
	evhttp_add_header(req->output_headers, "Cache-Control", "no-cache");

	//处理输出数据
	//evbuffer_add_printf(buf, "PROXY VERSION %s\n", ELEXSMRT_HTTP_PORT);
	read_file(decode_uri);
	evbuffer_add_printf(buf, "%s", filedata);
	//evbuffer_add_printf(buf, "------------------------------\n");
	//evbuffer_add_printf(buf, "YOU PASS NAME: %s\n", http_input_name);

	//返回code 200
	evhttp_send_reply(req, HTTP_OK, "OK", buf);

	//释放内存
	free(decode_uri);
	evhttp_clear_headers(&http_query);
	evbuffer_free(buf);
	free(filedata);
	filedata = NULL;



#if 0
	struct evbuffer *returnbuffer = evbuffer_new();

	printf("Requested:%s\n", evhttp_request_uri(req));
	evbuffer_add_printf(returnbuffer, "Requested: %s", evhttp_request_uri(req));
	evhttp_send_reply(req, HTTP_OK, "OK", returnbuffer);

	//evbuffer_add_printf(returnbuffer, "Thanks for the request!");
	//printf("\n------------------------------\n%d\n------------------------------\n", returnbuffer->total_len);
	//evhttp_send_reply(req, HTTP_OK, "Client", returnbuffer);
	evbuffer_free(returnbuffer);
	return;
#endif
}

void *event_ethernet_http(void *arg)
{
	net_manage *net_mg = (net_manage *)arg;

	DEBUG("I am in event_ethernet and as a http server thread!^_^ ");

	event_init();
	net_mg->http_server = evhttp_start(net_mg->sTnet.gCipaddr, ELEXSMRT_HTTP_PORT);
	evhttp_set_gencb(net_mg->http_server, evhttp_request_handler, NULL);

	fprintf(stderr, "Server started on port %d\n", ELEXSMRT_HTTP_PORT);
	event_dispatch();

	return(0);
}

/*---------------------------------------------------------------------------------------------------------*/

/* 网络事件监听并分配任务*/
void *event_ethernet(void *arg)
{
	int ret = -1;
#if __HTTP_ENABLE__
	pthread_t http_id;
#endif
	mngtask *mtask = (mngtask *)arg;
	net_manage *net_mg = NULL;

	net_mg = event_ethernet_listening(mtask->net, ELEXSMRT_ETH_NAME);
	if(!net_mg)
		return NULL;

#if __HTTP_ENABLE__
	pthread_create(&http_id, NULL, event_ethernet_http, (void *)net_mg);
#else
	/**
	 * set listen event to @net_base, it's function is procces client connect request
	 * and disribute a thread to receive and parse data which from client. 
	 *
	 * set timeout event to @net_base, it's function is get all device running data and
	 * update to the DB in server every 15 seconds. 
	 */
	net_mg->net_base = event_base_new();
	event_set(&net_mg->ev_listen, net_mg->listen, EV_READ|EV_PERSIST, event_ethernet_accepting, net_mg);
	event_base_set(net_mg->net_base, &net_mg->ev_listen);
	event_add(&net_mg->ev_listen, NULL);

	evtimer_set(&net_mg->ev_tout, event_ethernet_timeout, net_mg);
	evutil_timerclear(&net_mg->tv);
	net_mg->tv.tv_sec = 15;
	event_base_set(net_mg->net_base, &net_mg->ev_tout);
	event_add(&net_mg->ev_tout, &net_mg->tv);

	event_base_dispatch(net_mg->net_base);
#endif

	return 0;
}

