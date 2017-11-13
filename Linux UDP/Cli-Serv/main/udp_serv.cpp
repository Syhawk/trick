#include "wrapsock.h"
#include "error.h"


int main(int argc, char** argv) {
	int sockfd;
	sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

	Setsockresume(sockfd);

	struct sockaddr_in servaddr;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);
	Bind(sockfd, (SA*)&servaddr, sizeof(servaddr));

    udp_serv_echo(sockfd);

    Close(sockfd);

    return 0;
}

