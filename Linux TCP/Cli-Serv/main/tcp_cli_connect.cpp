#include "wrapsock.h"
#include "error.h"

int main(int argc, char** argv) {
    if (argc != 3) {
        err_quit("usage: tcp_connnect_cli <hostname/IP> <service/port>");
    }

	int sockfd;
	sockfd = Tcp_connect(argv[1], argv[2]);

    struct sockaddr_storage ss;
    socklen_t len;
    len = sizeof(ss);
    Getpeername(sockfd, (SA*)&ss, &len);
    printf("connect to %s\n", inet_ntoa(((struct sockaddr_in*)&ss)->sin_addr));
    getsockname(sockfd, (SA*)&ss, &len);
    printf("local %s\n", Sock_ntop((SA*)&ss));

    tcp_cli_select_echo(sockfd);

	Close(sockfd);

	return 0;
}
