

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>

int socketfd;

void signal_handler(int signo)
{
	if(signo == SIGINT){
		printf("service will be close!\n");
		close(socketfd);
		exit(-1);
	}
	if(signo == SIGCHLD){
		printf("children process exit!\n");
		wait(0);
	}
}

void out_client(struct sockaddr_in clientaddr)
{
	char buffer[16] = {'\0'};
	inet_ntop(AF_INET, &(clientaddr.sin_addr.s_addr),buffer, sizeof(clientaddr));
	int port = ntohs(clientaddr.sin_port);
	printf("client ip: %s, port: %d\n",buffer,port);
	
}

void do_service(int fd)
{
	printf("process pid %d do serive...\n",getpid());
	 long t = time(0);
	 char *tt = ctime(&t);
	 write(fd, tt, strlen(tt));
	close(fd);
}

int main(int argc, char *argv[])
{
	if(argc < 2){
		fprintf(stderr, "usage: %s port\n",
				argv[0]);
		exit(-1);
	}
	if(signal(SIGINT, signal_handler) == SIG_ERR||
	   signal(SIGCHLD, signal_handler) == SIG_ERR)
	{
		fprintf(stderr, "signal error: %s\n",
				strerror(errno));
		exit(-1);
	}

	//socket
	socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if(socketfd < 0){
		fprintf(stderr, "socket error:%s\n",
				strerror(errno));
		exit(-1);
	}
	//确定IP和端口
	struct sockaddr_in  sockaddr;
	memset(&sockaddr, 0, sizeof(sockaddr));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(atoi(argv[1]));
	sockaddr.sin_addr.s_addr = INADDR_ANY;
	//bind  绑定ip和端口
	if(bind(socketfd, (struct sockaddr*)&sockaddr,sizeof(sockaddr)) < 0)
	{
		fprintf(stderr, "bind error:%s\n",
				strerror(errno));
		exit(-1);		
	}
	//开始监听
	if(listen(socketfd, 50) < 0){
		fprintf(stderr, "listen error: %s\n",
				strerror(errno));
		exit(-1);
	}	
	while(1){
		struct sockaddr_in clientaddr;
		int len = sizeof(clientaddr);
		int fd = accept(socketfd, (struct sockaddr*)&clientaddr, &len);
		if(fd < 0){
			fprintf(stderr, "accept error:%s\n",
					strerror(errno));
			continue;
		}
		out_client(clientaddr);
		pid_t pid = fork();
		if(pid < 0){
			fprintf(stderr, "fork error:%s\n",
					strerror(errno));
			exit(-1);
		}else if(pid > 0){
			close(fd);
		}else{
			close(socketfd);
			do_service(fd);	
			break;
		}	
		//do_serive(fd);	
		//close(fd);
	}
}




