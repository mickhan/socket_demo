#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>

void error(char *msg)
{
	perror(msg);
	exit(1);
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

	listen(sockfd, 1);

	clilen = sizeof(cli_addr);
	while(1) {
		clifd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (clifd < 0)
			error("accept failed\n");

		bzero(buffer, 256);
		read(clifd, buffer, 255);
		printf("%s", buffer);

		write(clifd,"hello\n",6);

		close(clifd);
	}
	close(sockfd);
	return 0;
}
