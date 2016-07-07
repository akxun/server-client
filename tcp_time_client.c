

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>

int sockfd;
int main(int argc, char **argv)
{
	if(argc < 3){
		fprintf(stderr, "usage: %s ip port\n",
				argv[0]);
		exit(-1);
	}
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		fprintf(stderr, "socket error:%s\n",
				strerror(errno));
		exit(-1);
	}
	struct sockaddr_in  sockaddr;
	memset(&sockaddr, 0, sizeof(sockaddr));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(atoi(argv[2]));
	inet_pton(AF_INET, argv[1], &sockaddr.sin_addr.s_addr);

	if(connect(sockfd, (struct sockaddr*)&sockaddr,sizeof(sockaddr)) < 0){
		fprintf(stderr, "connect error:%s\n",
				strerror(errno));
		exit(-1);
	}

	char buffer[1024] = {'\0'};
	ssize_t n;
	if((n=read(sockfd, buffer, 1024)) < 0){
		fprintf(stderr, "read error:%s\n",
				strerror(errno));
	}else{
		if(write(STDOUT_FILENO, buffer, n) != n){
			fprintf(stderr, "write error:%s\n",
					strerror(errno));
		}
	}
	close(sockfd);

	return 0;
}



