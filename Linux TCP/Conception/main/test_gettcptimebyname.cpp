#include "error.h"
#include "wrapsock.h"

int main(int argc, char** argv) {
    if (argc != 3) {
        err_quit("usage: gettcptimebyname <hostname> <service>");
    }

    struct hostent* hp;
    struct in_addr inetaddr;
    struct in_addr *inetaddrp[2];
    struct in_addr** pptr;
    if ((hp = gethostbyname(argv[1])) == NULL) {
        if (inet_aton(argv[1], &inetaddr) == 0) {
            err_quit("host name error for %s: %s", argv[1], hstrerror(h_errno));
        } else {
            inetaddrp[0] = &inetaddr;
            inetaddrp[1] = NULL;
            pptr = inetaddrp;
        }
    } else {
        pptr = (struct in_addr**)hp->h_addr_list;
    }
    
    struct servent* sp;
    if ((sp = getservbyname(argv[2], "tcp")) == NULL) {
        err_quit("getservbyname error for %s", argv[2]);
    }

    int sockfd;
    struct sockaddr_in servaddr;
    for (; pptr != NULL; ++pptr) {
        sockfd = Socket(AF_INET, SOCK_STREAM, 0);

        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = sp->s_port;
        memcpy(&servaddr.sin_addr, *pptr, sizeof(struct in_addr));
        printf("trying %s\n", Sock_ntop((SA*)&servaddr));

        if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) == 0) {
            break;
        }
        err_ret("connect error");
        close(sockfd);
    }

    if (*pptr == NULL) {
        err_quit("unbale to connect");
    }

    int nread;
    char recvline[MAXLINE];
    while ((nread = Read(sockfd, recvline, sizeof(recvline)) > 0)) {
        recvline[nread] = 0;
        fputs(recvline, stdout);
    }

    exit(0);

    return 0;
}
