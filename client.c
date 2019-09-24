#include"common.h"
#include<stdio.h>
#include<string.h>
int admin_query(int sfd)
{
		//	ssize_t size;
	//	MSG msg;
	int cmd;
	while(1){
	printf("*************************************************************\n");
	printf("******* 1：按人名查找  2：查找所有 3：退出*******************\n");
	printf("*************************************************************\n");
	scanf("%d",&cmd);
		switch(cmd){
		case 1:
			break;
		case 2:
			break;
		case 3:
			return 1;
		}
	}
	/*
	   ssize_t size;
	   MSG msg;
	   char inbuf[32];
	   while(1){
	   fgets(inbuf, 32, stdin);
	   strcpy(msg.recvmsg,inbuf);
	   size = send(sfd,msg.recvmsg, sizeof(MSG), 0);
	   if(size < 0){
	   printf("send error\n");
	   }
	   }
	   */
}

int admin_func(int sfd)
{
	int cmd;
	char name[32];
	char passwd[32];
	printf("welcome admin\n");
next1:
	printf("input your name\n");
	scanf("%s",name);
	if(strcmp(name,"admin") != 0){
		printf("name error, try again\n");
		goto next1;
	}
next2:
	printf("input your password\n");
	scanf("%s",passwd);
	if(strcmp(passwd,"admin") != 0){
		printf("password  error, try again\n");
		goto next2;
	}
	while(1){
		printf("*******************************************************************\n");
		printf("1：查询  2：修改 3：添加用户  4：删除用户  5：查询历史记录  6：退出\n");
		printf("*******************************************************************\n");
		scanf("%d",&cmd);
		switch(cmd){
		case 1:
			admin_query(sfd);
			break;
		case 6:
			return 1;
		default:
			printf("error try again\n");
		}
	}
}
void user_func(int sfd)
{
	printf("welcome user\n");
}
void login_func(int sfd)
{
	int cmd;
	MSG msg;
	while(1){
		printf("*****************************************\n");
		printf("1:管理员登陆      2:用户登陆         3:退出\n");
		printf("*****************************************\n");
		scanf("%d",&cmd);
		switch(cmd){
		case 1:
			admin_func(sfd);
			break;
		case 2:
			user_func(sfd);
			break;
		case 3:
			close(sfd);
			exit(0);
		default:
			printf("error,please input again\n");
		}
	}
}
int main(int argc, const char *argv[])
{
	int sfd;
	sfd = socket(PF_INET, SOCK_STREAM, 0);
	if(sfd < 0){
		printf("socket error\n");
	}
	int len;
	struct sockaddr_in serveraddr;
	len = sizeof(serveraddr);
	serveraddr.sin_family = PF_INET;
	serveraddr.sin_port = htons(8888);
	serveraddr.sin_addr.s_addr = 0;
	int ret ;
	ret = connect(sfd, (struct sockaddr*)&serveraddr, len);
	if(ret < 0){
		printf("connect error\n");
	}
	login_func(sfd);
	/*
	   }
	   close(sfd);
	   */
	return 0;
	}
