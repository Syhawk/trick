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
	int maxindex = 0;
	struct pollfd client[N];
	client[0].fd = listenfd;
	client[0].events = POLLRDNORM;
	for (int i = 1; i < N; ++i) {
		client[i].fd = -1;
	}

	while (1) {
		int nready = poll(client, maxindex + 1, INFTIME);

		if (client[0].revents & POLLRDNORM) {
			struct sockaddr_in cliaddr;
			socklen_t clilen;
			int connfd;
			clilen = sizeof(cliaddr);

			connfd = Accept(listenfd, (SA*)&cliaddr, &clilen);

			int i = 0;
			for (; i < N; ++i) {
				if (client[i].fd < 0) {
					client[i].fd = connfd;
					client[i].events = POLLRDNORM;
					break;
				}
			}

			if (i == N) {
				err_quit("too many clients");
			}

			maxindex = std::max(maxindex, i);

			if (--nready <= 0) {
				continue;
			}
		}

		char buf[MAXLINE];
		for (int i = 0; i <= maxindex; ++i) {
			int connfd = client[i].fd;
			if (connfd < 0) {
				continue;
			}

			if (client[i].revents & (POLLRDNORM | POLLERR)) {
				int n;
				bzero(buf, sizeof(buf));
				if ((n = Read(connfd, buf, sizeof(buf))) <= 0) {
					if (n < 0 && ECONNRESET != errno) {
						err_sys("Read error");
					}
					Close(connfd);
					client[i].fd = -1;
					printf("client closed\n");
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
