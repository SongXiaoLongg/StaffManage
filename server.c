#include<stdio.h>
#include"common.h"
#include <sys/types.h>         
#include <sys/socket.h>
#include<netinet/in.h>
#include <sys/time.h>
#include <unistd.h>
#include<stdlib.h>
#include<sqlite3.h>
int deal_msg_func(int sfd,MSG msg,sqlite3 *db)
{
	char *errmsg;
	char sql[128];
	char **rep;
	int n_row;
	int n_colum;
	int i,j;
	switch(msg.msgtype){
		case ADMIN_QUERY:
			if(msg.flags == 1){
				printf("按名字查询\n");
				memset(sql,0,sizeof(sql));
				sprintf(sql,"select * from staff where name = '%s'",msg.info.name);
				printf("%s\n",sql);
				if(sqlite3_get_table(db,sql,&rep,&n_row,&n_colum,&errmsg)!= SQLITE_OK){
					printf("find name error\n");
				}	
				for(i=0;i<n_row+1;i++){
					for(j=0;j<n_colum;j++){
						printf("%-15s",*rep);
						strcpy(msg.recvmsg,*rep++);
						send(sfd,&msg,sizeof(MSG),0);
					}
					printf("\n");
					strcpy(msg.recvmsg,"\n");
					send(sfd,&msg,sizeof(MSG),0);
				}
				strcpy(msg.recvmsg,"over");
				send(sfd,&msg,sizeof(MSG),0);

			}else{
				printf("查找所有\n");
				memset(sql,0,sizeof(sql));
				if(sqlite3_get_table(db,"select * from staff",&rep,&n_row,&n_colum,&errmsg)!= SQLITE_OK){
					printf("find all error\n");	
				}
				for(i=0;i<n_row+1;i++){
					for(j=0;j<n_colum;j++){
						printf("%-15s",*rep);
						strcpy(msg.recvmsg,*rep++);
						send(sfd,&msg,sizeof(MSG),0);
					}
					printf("\n");
				strcpy(msg.recvmsg,"\n");
				send(sfd,&msg,sizeof(MSG),0);
				}
					strcpy(msg.recvmsg,"over");
					send(sfd,&msg,sizeof(MSG),0);

			}
			break;
		case ADMIN_ADDUSER:
			memset(sql,0,sizeof(sql));
			sprintf(sql,"insert into staff values(%d,'%s','%s',%d,'%s',%lf)",msg.info.no,msg.info.name,msg.info.passwd,msg.info.age,msg.info.phone,msg.info.salary);
			printf("%s\n",sql);
			if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != SQLITE_OK){
				printf("%s\n",errmsg);
			}
			break;
	}
	return 1;
}

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
	//sqlite_func();
	sqlite3 *db;
	char *errmsg;
	if(sqlite3_open("staffinfo.db",&db) != SQLITE_OK){
		printf("open Failed:%s\n", sqlite3_errmsg(db));
	}else{
		printf("open to staffinfo seccess\n");
	}
	char * sql;
	sql = "create table staff (no INT,name TEXT,password TEXT,age INT,phone TEXT,salary REAL )";
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != SQLITE_OK){
		printf("exec Failed:%s\n",sqlite3_errmsg(db));
	}
	sql = "create table hisinfo(time char,name char,word char)";
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != SQLITE_OK){
		printf("exec Failed:%s\n",sqlite3_errmsg(db));
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
					size = recv(i,&msg,sizeof(MSG),0 );
					if(size < 0){
						printf("recv error\n");
						FD_CLR(i, &readfd);
						close(i);
						continue;
					}
					deal_msg_func(i,msg,db);
				}
			}
		}
	}
	return 0;
}
