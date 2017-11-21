#include "wrapsock.h"
#include "error.h"

#define N FD_SETSIZE

int main(int argc, char** argv) {
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

	int maxfd = listenfd;
	int maxindex = -1;
	int client[N];
	for (int i = 0; i < N; ++i) {
		client[i] = -1;
	}
	fd_set rset;
	fd_set allset;
	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);

	while (1) {
		rset = allset;
		int nready = Select(maxfd + 1, &rset, NULL, NULL, NULL);

		if (FD_ISSET(listenfd, &rset)) {
			struct sockaddr_in cliaddr;
			socklen_t clilen;
			int connfd;
			clilen = sizeof(cliaddr);

			connfd = Accept(listenfd, (SA*)&cliaddr, &clilen);

			int i = 0;
			for (; i < N; ++i) {
				if (client[i] < 0) {
					client[i] = connfd;
					break;
				}
			}

			if (i == N) {
				err_quit("too many clients");
			}

			FD_SET(connfd, &allset);
			maxfd = std::max(maxfd, connfd);
			maxindex = std::max(maxindex, i);

			if (--nready <= 0) {
				continue;
			}
		}

		char buf[MAXLINE];
		for (int i = 0; i <= maxindex; ++i) {
			int connfd = client[i];
			if (connfd < 0) {
				continue;
			}

			if (FD_ISSET(connfd, &rset)) {
				int n;
				bzero(buf, sizeof(buf));
				if ((n = Read(connfd, buf, sizeof(buf))) <= 0) {
					Close(connfd);
					FD_CLR(connfd, &rset);
					client[i] = -1;
				} else {
					printf("buf is %s", buf);
					Writen(connfd, buf, n);
				}

				if (--nready <= 0) {
					break;
				}
			}
		}
	}

	Close(listenfd);

	return 0;
}
