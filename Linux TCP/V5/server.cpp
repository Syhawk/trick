/*
*   1. int socket(int domain, int type, int protocol);
*   2. int bind(int sockfd, const struct sockaddr* addr, socklen_t addrlen);
*   3. int listen(int sockfd, int backlog);
*   4. int accept(int sockfd, struct sockaddr* addr, socklen_t* addrlen);
*   5. int accept_timeout(int sockfd, struct sockaddr* addr, unsigned int wait_seconds);
*   6. ssize_t readn(int fd, void *buf, size_t count);
*   7. ssize_t writen(int fd, const void *buf, size_t count);
*   8. ssize_t readline(int sockfd, void *buf, size_t len);
*   9. int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
*   10. int poll(struct pollfd *fds, nfds_t nfds, int timeout);
*/

#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
#include <poll.h>

#include "interface.h"

using namespace std;

#define BUFFER_SIZE 1024
#define FILE_DESCRIPTOR_NUM 2048

struct Point {
    int ad;
    int no;
    sockaddr_in remote_ip;
    Point() {}
};

int cnt;
struct Point clients[FILE_DESCRIPTOR_NUM];

void process(void* arg, int size) {
    Point* p = (Point*)arg;
    if (size < 0) {
        printf("------------client %d read data error!------------\n", p->no);
    }
    printf("------------client %d close!------------\n", p->no);
    close(p->ad);
    p->ad = -1;
}

ssize_t broadcast(void* arg) {
    Point* p = (Point*)arg;
    char buf[BUFFER_SIZE] = {0};

    int size = readline(p->ad, buf, sizeof(buf));
    if (size <= 0) {
        process(arg, size);
        return -1;
    }

    printf("read data from client: %s\n", inet_ntoa(p->remote_ip.sin_addr));
    printf("data is: %s", buf);

    // Write message to all client.
    for (int i = 1; i < cnt; ++i) {
        if (clients[i].ad != p->ad && clients[i].ad >= 0) {
            writen(clients[i].ad, buf, strlen(buf));
        }
    }

    return 0;
}

int main() {
    struct sockaddr_in server_ip;
    int server_len;
    socklen_t remote_len;
    int err;
    int sd;

    // Create socket.
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd == -1) {
        ERR_EXIT("create socket error");
    }

    // Set server can restart right away.
    int on = 1;
    err = setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if (err == -1) {
        close(sd);
        ERR_EXIT("setsockopt error");
    }

    // Set ip address and port.
    server_ip.sin_family = AF_INET;
    server_ip.sin_port = htons(5678);
    server_ip.sin_addr.s_addr = htonl(INADDR_ANY);
    memset(server_ip.sin_zero, 0, sizeof(server_ip.sin_zero));
    
    // Bind ip address and port to socket.
    err = bind(sd, (struct sockaddr*)(&server_ip), sizeof(struct sockaddr));
    if (err == -1) {
        close(sd);
        ERR_EXIT("bind error");
    }

    // Set setver's max connect number.
    err = listen(sd, SOMAXCONN);
    if (err == -1) {
        close(sd);
        ERR_EXIT("listen error");
    }

    // Get client ip address's size.
    remote_len = sizeof(struct sockaddr);

    // Poll calling param.
    struct pollfd fds[FILE_DESCRIPTOR_NUM];
    cnt = 1;
    int timeout = -1;
    
    fds[0].fd = sd;
    fds[0].events = POLLIN;

    while (cnt < FILE_DESCRIPTOR_NUM) {
        int nready = poll(fds, cnt, timeout);

        if (nready == -1) {
            if (errno == EINTR) {
                continue;
            }
            perror("select run error");
            break;
        }
        if (nready == 0) {
            continue;
        }

        if (fds[0].revents & POLLIN) {
            // Wait client's request, if request getted, return a new socket;
            // Server connect client with new socket.
            // clients[cnt].ad = accept(sd, (struct sockaddr*)(&(clients[cnt].remote_ip)), &remote_len);
            unsigned int wait_seconds = 3;
            clients[cnt].ad = accept_timeout(sd, &(clients[cnt].remote_ip), wait_seconds);
            if (clients[cnt].ad == -1) {
                printf("accept error, errno is %d\n", errno);
                continue;
            }

            // Find the min avalible position.
            int index = 1;
            for (; index < cnt; ++index) {
                if (clients[index].ad < 0) {
                    swap(clients[index], clients[cnt]);
                    break;
                }
            }

            clients[index].no = index + 1;
            nready -= 1;
            
            fds[cnt].fd = clients[index].ad;
            fds[cnt].events = POLLIN;

            printf("------------client %d start!------------\n", index);
            printf("ip is %s, port is %d.\n", inet_ntoa(clients[index].remote_ip.sin_addr), ntohs(clients[index].remote_ip.sin_port));
            cnt += (cnt == index);
        }

        // Here has other I/O socket to read data.
        for (int i = 1; i < cnt && nready > 0; ++i) {
            if (fds[i].fd == -1 || !(fds[i].revents & POLLIN)) {
                continue;
            }

            int conn = clients[i].ad;
            err = broadcast((void*)(&clients[i]));
            if (err == -1) {
                fds[i].fd = -1;
            }
            nready -= 1;
        }
    }

    for (int i = 0; i < cnt; ++i) {
        if (clients[i].ad >= 0) {
            close(clients[i].ad);
        }
    }

    close(sd);

    return 0;
}
