#include "unp.h"
#include "wrapsock.h"

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

	Signal(SIGCHLD, sig_chld);

	while (1) {
		struct sockaddr_in cliaddr;
		socklen_t clilen;
		int connfd;
		clilen = sizeof(cliaddr);

		connfd = Accept(listenfd, (SA*)&cliaddr, &clilen);

		pid_t chldpid;
		if ((chldpid = fork()) == 0) {
			close(listenfd);
			serv_echo(connfd);
			close(connfd);
			exit(0);
		}
		close(connfd);
	}

	return 0;
}
