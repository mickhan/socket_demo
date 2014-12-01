#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<signal.h>
#include<sys/wait.h>

void error(char *msg)
{
	perror(msg);
	exit(1);
}

void sig_child(int signo)
{
	pid_t pid;
	int stat;
	while((pid=waitpid(-1,&stat,WNOHANG))>0)
		printf("child %d terminated\n", pid);
	return ;
}

int main()
{
	int sockfd,clifd,clilen;
	struct sockaddr_in serv_addr,cli_addr;
	char buffer[256];

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("open socket failed\n");

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(1080);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		error("binding failed\n");

	listen(sockfd, 511);
	signal(SIGCHLD, sig_child);
	clilen = sizeof(cli_addr);
	while(1) {
		clifd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (clifd < 0)
			error("accept failed\n");
		if (fork() == 0) {
			bzero(buffer, 256);
			read(clifd, buffer, 255);
			printf("%s", buffer);

			write(clifd,"hello\n",6);
			exit(0);
		}
		close(clifd);
	}
	close(sockfd);
	return 0;
}
