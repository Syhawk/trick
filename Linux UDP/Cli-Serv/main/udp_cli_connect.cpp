#include "unp.h"
#include "wrapsock.h"
#include "error.h"

int main(int argc, char** argv) {
    if (argc != 3) {
        err_quit("usage: udp_cli_client <hostname/IP> <service/port>");
    }

	int sockfd;
	sockfd = Udp_connect(argv[1], argv[2]);

	udp_cli_echo(sockfd);

	Close(sockfd);

	return 0;
}
