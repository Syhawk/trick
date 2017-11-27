#include "unp.h"
#include "wrapsock.h"
#include "error.h"

int main(int argc, char** argv) {
    if (argc != 3) {
        err_quit("usage: udp_cli_client <hostname/IP> <service/port>");
    }

	int sockfd;
    struct sockaddr* sa;
    socklen_t sa_len;
	sockfd = Udp_client(argv[1], argv[2], &sa, &sa_len);

    printf("sending to %s\n", Sock_ntop(sa));

	udp_cli_echo(sockfd, sa, sa_len);

	Close(sockfd);

	return 0;
}
