/*
*   1. int socket(int domain, int type, int protocol);
*   2. int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
*   3. int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
*   5. ssize_t read(int fd, void *buf, size_t count);
*   6. ssize_t write(int fd, const void *buf, size_t count);
*   7. writen, readline.
*
*   man 2 stat
*   man unix
*/

#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/un.h>

#include "interface.h"
#include "common.h"

void echo_cli(int sock) {
    char sendbuf[MSG_LEN] = {0};
    char recvbuf[MSG_LEN] = {0};
    
    while (fgets(sendbuf, sizeof(sendbuf), stdin) != NULL) {
        int err = writen(sock, sendbuf, strlen(sendbuf));
        if (err == -1 && errno != EINTR) {
            ERR_EXIT("client write error.");
        }
        if (err == 0) {
            printf("server close.\n");
            break;
        }

        err = readline(sock, recvbuf, sizeof(recvbuf));
        if (err == -1) {
            if (errno == EINTR) {
                continue;
            }
            ERR_EXIT("clinet read error");
        }
        if (err == 0) {
            printf("server close.\n");
            break;
        }
        
        fputs(recvbuf, stdout);
        memset(sendbuf, 0, sizeof(sendbuf));
        memset(recvbuf, 0, sizeof(recvbuf));
    }

    close(sock);
}

int main() {
    // Create socket.
    int sock = socket(PF_UNIX, SOCK_STREAM, 0);
    if (sock == -1) {
        ERR_EXIT("client create socket error");
    }

    // Set family and path.
    struct sockaddr_un server_ip;
    memset(&server_ip, 0, sizeof(server_ip));
    server_ip.sun_family = AF_UNIX;
    strcpy(server_ip.sun_path, "/tmp/test_sock");

    // Connect to server.
    int err = connect(sock, (struct sockaddr*)(&server_ip), sizeof(struct sockaddr));
    if (err < 0) {
        close(sock);
        ERR_EXIT("client connect error");
    }

    echo_cli(sock);

    return 0;
}
