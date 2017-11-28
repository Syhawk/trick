#include "unp.h"
#include "wrapsock.h"
#include "error.h"

int main(int argc, char** argv) {
    if (argc < 2 || argc > 3) {
        err_quit("usage: daytimetcpserv2 [<host>] <service or port>");
    }

    Daemon_init(argv[0], 0);

	int listenfd;
    socklen_t addrlen;
    if (argc == 2) {
        listenfd = Tcp_listen(NULL, argv[1], &addrlen);
    } else if (argc == 3) {
        listenfd = Tcp_listen(argv[1], argv[2], &addrlen);
    }

    socklen_t clilen;
    struct sockaddr_storage cliaddr;
    int connfd;
    while (1) {
        clilen = sizeof(cliaddr);
        connfd = Accept(listenfd, (SA*)&cliaddr, &clilen);
        printf("connection from %s\n", Sock_ntop((SA*)&cliaddr));

		tcp_serv_echo(connfd);
    }

	return 0;
}
