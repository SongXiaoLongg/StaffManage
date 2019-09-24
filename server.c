#include<stdio.h>
#include"common.h"
#include <sys/types.h>         
#include <sys/socket.h>
#include<netinet/in.h>
#include <sys/time.h>
#include <unistd.h>

int main(int argc, const char *argv[])
{
	int sfd;
	int len;
	sfd = socket(PF_INET, SOCK_STREAM, 0);
	if(sfd < 0){
		printf("socket error\n");
	}
	struct sockaddr_in serveraddr;
	len = sizeof(serveraddr);
	serveraddr.sin_family = PF_INET;
	serveraddr.sin_port = htons(8888);
	serveraddr.sin_addr.s_addr = 0;

	int ret;
	ret = bind(sfd, (struct sockaddr*)&serveraddr, sizeof(struct sockaddr_in));
	if(ret < 0){
		printf("bind error\n");
	}

	ret = listen(sfd, 10);
	if(ret < 0){
		printf("listen error\n");
	}

	int acpfd;
	struct sockaddr_in clientaddr;
	fd_set readfd, temptd;//定义监听文件描述符的集合
	FD_ZERO(&readfd);
	FD_ZERO(&temptd);
	FD_SET(sfd,&readfd);
	int maxfd;
	maxfd = sfd + 1;
	int i;
	while(1)
	{
		temptd = readfd;
		printf("blocking\n");
		ret = select(maxfd, &temptd,NULL,NULL,NULL);
		if(FD_ISSET(sfd,&temptd)){
			acpfd = accept(sfd, (struct sockaddr*)&clientaddr, &len);
			printf("acpfd=%d\n",acpfd);
			FD_SET(acpfd, &readfd);
			maxfd = (acpfd+1 > maxfd) ? (acpfd+1) : maxfd;//注意文件描述符集合的最大值
		}else{
			for(i=3;i<maxfd;i++){
				if(FD_ISSET(i,&temptd)){
					ssize_t size;
					MSG msg;
					size = recv(i,msg.recvmsg,sizeof(msg),0 );
					if(size < 0){
						printf("recv error\n");
						FD_CLR(i, &readfd);
						close(i);
						maxfd = (i+1 == maxfd)?(maxfd -1):maxfd;
						continue;
					}
					printf("%s\n",msg.recvmsg);
				}
			}
		}
	}
	return 0;
}
