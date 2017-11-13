#include "unp.h"
#include "wrapsock.h"

int main(int argc, char** argv) {
	int sockfd;
	sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

	struct sockaddr_in servaddr;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	udp_cli_echo(sockfd, (SA*)&servaddr, sizeof(servaddr));

	Close(sockfd);

	return 0;
}
