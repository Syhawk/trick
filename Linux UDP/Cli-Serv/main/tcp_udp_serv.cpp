#include "wrapsock.h"
#include "error.h"


int main(int argc, char** argv) {
    // TCP
	int listenfd;
	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	Setsockresume(listenfd);

	struct sockaddr_in servaddr;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);
	Bind(listenfd, (SA*)&servaddr, sizeof(servaddr));

    Listen(listenfd, LISTENQ);

    // UDP
    int udpfd;
    udpfd = Socket(AF_INET, SOCK_DGRAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    Bind(udpfd, (SA*)&servaddr, sizeof(servaddr));

    // Left common section.
    Signal(SIGCHLD, sig_chld);

    struct sockaddr_in cliaddr;
    socklen_t clilen;

    int pid;

    fd_set rset;
    int maxfd;
    int nready;

    char buf[MAXLINE];

    FD_ZERO(&rset);
    maxfd = std::max(listenfd, udpfd);

    while (1) {
        FD_SET(listenfd, &rset);
        FD_SET(udpfd, &rset);

        if ((nready = Select(maxfd + 1, &rset, NULL, NULL, NULL)) < 0) {
            if (EINTR == errno) {
                continue;
            } else {
                err_sys("Select error");
            }
        }

        if (FD_ISSET(listenfd, &rset)) {
            clilen = sizeof(cliaddr);
            int connfd = Accept(listenfd, (SA*)&cliaddr, &clilen);

            if ((pid = Fork()) == 0) {
                Close(listenfd);
                Close(udpfd);
                tcp_serv_echo(connfd);
                exit(0);
            }
            Close(connfd);
        }

        if (FD_ISSET(udpfd, &rset)) {
            bzero(buf, sizeof(buf));

            clilen = sizeof(cliaddr);
            int nread = Recvfrom(udpfd, buf, sizeof(buf), 0, (SA*)&cliaddr, &clilen);
            printf("udp buf is %s", buf);
            Sendto(udpfd, buf, nread, 0, (SA*)&cliaddr, clilen);
        }
    }

    Close(listenfd);
    Close(udpfd);

    return 0;
}

