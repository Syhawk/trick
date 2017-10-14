/*
*   1. int socket(int domain, int type, int protocol);
*   2. int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
*   3. ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen);
*   4. ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);
*/

#include <string.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>

#include "interface.h"

#define BUFFER_SIZE 1024

void cli(int sock) {
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5555);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(serv_addr.sin_zero, 0, sizeof(serv_addr.sin_zero));
    
    // If use connect, can use send to send data.
    //connect(sock, (struct sockaddr*)(&serv_addr), sizeof(struct sockaddr));

    char send_buf[BUFFER_SIZE] = {0};
    char recv_buf[BUFFER_SIZE] = {0};

    while (fgets(send_buf, sizeof(send_buf), stdin) != NULL) {
        int err = sendto(sock, send_buf, strlen(send_buf), 0, (struct sockaddr*)(&serv_addr), sizeof(struct sockaddr));
        if (err < 0) {
            if (errno == EINTR) {
                continue;
            }
            ERR_EXIT("sendto error");
        }

        err = recvfrom(sock, recv_buf, sizeof(recv_buf), 0, NULL, NULL);
        if (err < 0) {
            if (errno == EINTR) {
                continue;
            }
            ERR_EXIT("recvfrom error.");
        }
        printf("recv data is: %s", recv_buf);

        memset(send_buf, 0, sizeof(send_buf));
        memset(recv_buf, 0, sizeof(recv_buf));
    }
}

int main() {
    // Create socket.
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        ERR_EXIT("create socket error");
    }

    cli(sock);
    close(sock);

    return 0;
}
