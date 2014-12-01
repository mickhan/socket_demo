#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<sys/select.h>

void error(char *msg)
{
	perror(msg);
	exit(1);
}

int readfd(int fd)
{
	printf("reading fd %d\n", fd);
	char buffer[256];
	bzero(buffer, 256);
	read(fd, buffer, 255);
	printf("%s", buffer);
	return 0;
}

int main()
{
	int sockfd,clifd,clilen,i;
	struct sockaddr_in serv_addr,cli_addr;
	fd_set fdset;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("open socket failed\n");

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(1080);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		error("binding failed\n");

	listen(sockfd, 511);

	FD_ZERO(&fdset);
	FD_SET(sockfd,&fdset);
	while(1)
	{
		select(FD_SETSIZE,&fdset,NULL,NULL,NULL);
		for (i=0;i<=FD_SETSIZE;i++)
		{
			if (FD_ISSET(i,&fdset))
			{
				printf("into the loop\n");
				if (i == sockfd)
				{
					printf("new connection\n");
					clilen = sizeof(cli_addr);
					clifd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
					if (clifd < 0)
						error("accept failed\n");
					FD_SET(clifd,&fdset);
				}
				else
				{
					//printf("%d is ready\n",i);
					readfd(i);
					//close(i);
					//FD_CLR(i,&fdset);
				}
			}
		}
	}
	close(sockfd);
	return 0;
}
