/*
*   1. int socket(int domain, int type, int protocol);
*   2. int bind(int sockfd, const struct sockaddr* addr, socklen_t addrlen);
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

void serv(int sock) {
    char buf[BUFFER_SIZE];
    struct sockaddr_in peer_addr;
    socklen_t peer_len;

    while (1) {
        peer_len = sizeof(peer_addr);
        memset(buf, 0, sizeof(buf));
        int err = recvfrom(sock, buf, sizeof(buf), 0, (struct sockaddr*)(&peer_addr), &peer_len);
        if (err < 0) {
            if (errno == EINTR) {
                continue;
            }
            ERR_EXIT("recvfrom error");
        } else if (err > 0) {
            printf("data is: %s", buf);
            sendto(sock, buf, strlen(buf), 0, (struct sockaddr*)(&peer_addr), peer_len);
        }
    }
}

int main() {
    // Create socket.
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        ERR_EXIT("create socket error");
    }

    // Set ip address and port.
    struct sockaddr_in server_ip;
    server_ip.sin_family = AF_INET;
    server_ip.sin_port = htons(5555);
    server_ip.sin_addr.s_addr = htonl(INADDR_ANY);
    memset(server_ip.sin_zero, 0, sizeof(server_ip.sin_zero));

    // Bind ip address and port to socket.
    int err = bind(sock, (struct sockaddr*)(&server_ip), sizeof(struct sockaddr));
    if (err == -1) {
        close(sock);
        ERR_EXIT("bind error");
    }

    serv(sock);
    close(sock);

    return 0;
}
