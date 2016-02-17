#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT        25341

char *data = "This is a test, bye!";
int icount = 0;

int init_tcp(void)
{
	int sfd = -1;
	int ret = -1;
	int len;
	char rbuf[1024] = {0};
	struct sockaddr_in serveraddr = {0};

	sfd = socket(AF_INET,SOCK_STREAM,0);
	if (sfd < 0){
		perror("create socket error");
		return sfd;
	} 

	len = sizeof(struct sockaddr_in);
	serveraddr.sin_family = AF_INET;
	//serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	inet_aton("192.168.1.221" , &(serveraddr.sin_addr));
	serveraddr.sin_port = htons(PORT);

	ret = connect(sfd, (struct sockaddr *)&serveraddr, len);
	if(ret < 0){
		perror("connect");
		close(sfd);
	}

#if 1
	while(icount++ < 5){
		sleep(1);
		ret = send(sfd, data, strlen(data), 0);
		if(ret < 0){
			perror("send");
			close(sfd);
		}
		else{
			printf("send succesful!\n");
	
			recv(sfd, rbuf, 1024, 0);
			printf("recv from server:%s\n", rbuf);
			//close(sfd);
		}
	}

	icount = 0;
	write(stderr, "waiting.", 8);
	while(icount++ < 10){
		write(stderr, ".", 1);
		sleep(1);
	}
	icount = 0;
#endif

	close(sfd);
	return 0;
}

int main(void)
{
	int i = 0;

	while(i++ < 100){
		sleep(1);
		init_tcp();
	}

	return 0;
}

