#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<sys/epoll.h>

#define MAXEVENTS 100

void error(char *msg)
{
	perror(msg);
	exit(1);
}

int readfd(int fd)
{
//	printf("reading fd %d\n", fd);
	char buffer[256];
	bzero(buffer, 256);
	read(fd, buffer, 255);
	printf("%s", buffer);
	return 0;
}

int main()
{
	int sockfd,clifd,efd,clilen,i,numevents;
	struct sockaddr_in serv_addr,cli_addr;
	struct epoll_event event;
	struct epoll_event events[MAXEVENTS];

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("open socket failed\n");

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(1080);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		error("binding failed\n");

	listen(sockfd, 511);

	efd = epoll_create(1024);

	event.data.fd = sockfd;
	event.events = EPOLLIN;
	epoll_ctl(efd, EPOLL_CTL_ADD, sockfd, &event);

	while(1)
	{
		numevents = epoll_wait(efd, events, MAXEVENTS, -1);
		for (i=0;i<=numevents;i++)
		{
			if ((events[i].events & EPOLLERR) ||
			      (events[i].events & EPOLLHUP) ||
			      (!(events[i].events & EPOLLIN)))
			{
				close (events[i].data.fd);
			        continue;	
			}
			else if (events[i].events & EPOLLRDHUP)
			{
				printf("close fd %d\n",events[i].data.fd); 
				close (events[i].data.fd);
			        continue;	
			}
			else if (events[i].data.fd == sockfd)
			{
				clilen = sizeof(cli_addr);
				clifd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
				printf("new connection fd %d\n",clifd);
				if (clifd < 0)
					error("accept failed\n");
				event.data.fd = clifd;
				event.events = EPOLLIN;
				epoll_ctl(efd, EPOLL_CTL_ADD, clifd, &event);
			        continue;	
			}
			else
			{
				//printf("%d is ready\n",i);
				readfd(events[i].data.fd);
				//close(events[i].data.fd);
			        continue;	
			}
		}
	}
	close(sockfd);
	return 0;
}
