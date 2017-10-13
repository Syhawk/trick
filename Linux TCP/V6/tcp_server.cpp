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

*   epoll api;
*   10. int epoll_create1(int flags);
*   11. int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
*   12. int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
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
#include <sys/epoll.h>
#include <vector>
#include <map>

#include "interface.h"

using namespace std;

#define BUFFER_SIZE 1024

struct Point {
    int ad;
    int no;
    sockaddr_in remote_ip;
    Point() {}
};

typedef vector<struct epoll_event> EventList;
map<int, struct Point> clients;

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
    for (map<int, Point>::iterator itr = clients.begin(); itr != clients.end(); ++itr) {
        if (itr->second.ad != p->ad) {
            writen(itr->second.ad, buf, strlen(buf));
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

    // Epoll calling param.
    int epollfd;
    epollfd = epoll_create1(EPOLL_CLOEXEC);
    
    struct epoll_event event;
    event.data.fd = sd;
    event.events = EPOLLIN | EPOLLET;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, sd, &event);

    EventList events(16);
    int timeout = -1;
    unsigned int wait_seconds = 3;
    while (1) {
        int nready = epoll_wait(epollfd, &*events.begin(), static_cast<int>(events.size()), timeout);

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
        
        if ((size_t)nready == events.size()) {
            events.resize(events.size() * 2);
        }

        for (int i = 0; i < nready; ++i) {
            if (events[i].data.fd == sd) {
                // Wait client's request, if request getted, return a new socket;
                // Server connect client with new socket.
                Point client;
                int conn = accept_timeout(sd, &(client.remote_ip), wait_seconds);
                if (conn == -1) {
                    printf("accept error, errno is %d\n", errno);
                    continue;
                }
                client.ad = conn;
                client.no = clients.size() + 1;
                clients[conn] = client;

                active_nonblock(conn);
                event.data.fd = conn;
                event.events = EPOLLIN | EPOLLET;
                epoll_ctl(epollfd, EPOLL_CTL_ADD, conn, &event);

                printf("------------client %d start!------------\n", client.no);
                printf("ip is %s, port is %d.\n", inet_ntoa(client.remote_ip.sin_addr), ntohs(client.remote_ip.sin_port));
            } else if (events[i].events & EPOLLIN) {
                int conn = events[i].data.fd;
                err = broadcast((void*)(&clients[conn]));
                if (err == -1) {
                    event = events[i];
                    epoll_ctl(epollfd, EPOLL_CTL_DEL, conn, &event);
                    clients.erase(conn);
                }
            }
        }
    }

    for (map<int, Point>::iterator itr = clients.begin(); itr != clients.end(); ++itr) {
        close(itr->first);
    }

    close(sd);

    return 0;
}
