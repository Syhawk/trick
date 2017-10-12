/*
*   1. int socket(int domain, int type, int protocol);
*   2. int bind(int sockfd, const struct sockaddr* addr, socklen_t addrlen);
*   3. int listen(int sockfd, int backlog);
*   4. int accept(int sockfd, struct sockaddr* addr, socklen_t* addrlen);
*   5. read, write
*   6. readn, writen: For solving problem buffer insufficient, implement new method by old method.
*   7. recv_peek, readline: For solving socket(TCP)'s sticky package problem, use special suffix('\n').
*   8. select: For multiple client connecting to server by only one thread.
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
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;

#define BUFFER_SIZE 100

struct Point {
    int ad;
    int no;
    sockaddr_in remote_ip;
    Point() {}
};

int cnt;
struct Point clients[FD_SETSIZE];

// For solving problem buffer insufficient, implement new method by old method.
ssize_t readn(int fd, void *buf, size_t count) {
    size_t nleft = count;
    ssize_t nread;
    char* bufp = (char*)buf;

    while (nleft > 0) {
        if ((nread = read(fd, bufp, nleft)) < 0) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        } else if (nread == 0) {
            return count - nleft;
        }

        nleft -= nread;
        bufp += nread;
    }

    return count - nleft;
}

ssize_t writen(int fd, const void *buf, size_t count) {
    size_t nleft = count;
    ssize_t nwrite;
    char* bufp = (char*)buf;

    while (nleft > 0) {
        if ((nwrite = write(fd, bufp, nleft)) < 0) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        } else if (nwrite == 0) {
            return count - nleft;
        }

        nleft -= nwrite;
        bufp += nwrite;
    }

    return count - nwrite;
}

ssize_t recv_peek(int sockfd, void *buf, size_t len) {
    int nread;
    while (1) {
        nread = recv(sockfd, buf, len, MSG_PEEK);
        if (nread == -1 && errno == EINTR) {
            continue;
        }
        return nread;
    }
    return -1;
}

// For solving socket(TCP)'s sticky package problem, use special suffix('\n').
ssize_t readline(int sockfd, void *buf, size_t len) {
    int nleft = len;
    char* bufp = (char*)buf;
    int nread;
    while (nleft > 0) {
        nread = recv_peek(sockfd, bufp, nleft);
        if (nread < 0) {
            return -1;
        } else if (nread == 0) {
            return len - nleft;
        }

        int index = 0;
        for (; index < nread && bufp[index] != '\n'; ++index) {}
        bool flg = (index == nread);
        index += (!flg);

        nread = readn(sockfd, bufp, index);
        if (nread != index || nread > nleft) {
            return -1;
        }

        bufp += nread;
        nleft -= nread;
        if (!flg) {
            return len - nleft;
        }
    }
    
    return -1;
}

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
    for (int i = 0; i < cnt; ++i) {
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
        printf("create socket failed, errno is %d.\n", errno);
        return 0;
    }

    // Set ip address and port.
    server_ip.sin_family = AF_INET;
    server_ip.sin_port = htons(5678);
    server_ip.sin_addr.s_addr = htonl(INADDR_ANY);
    memset(server_ip.sin_zero, 0, sizeof(server_ip.sin_zero));

    // Set server can restart right away.
    int on = 1;
    err = setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if (err == -1) {
        printf("setsockopt error, errno is %d.\n", errno);
        close(sd);
        return 0;
    }
    
    // Bind ip address and port to socket.
    err = bind(sd, (struct sockaddr*)(&server_ip), sizeof(struct sockaddr));
    if (err == -1) {
        printf("bind error, errno is %d.\n", errno);
        close(sd);
        return 0;
    }
    
    // Set setver's max connect number.
    err = listen(sd, SOMAXCONN);
    if (err == -1) {
        printf("listen error, errno is %d.\n", errno);
        close(sd);
        return 0;
    }


    cnt = 0;
    // Get client ip address's size.
    remote_len = sizeof(struct sockaddr);

    // Select calling param.
    int max_fd = sd;
    fd_set rset;
    fd_set aset;
    FD_ZERO(&aset);
    FD_SET(sd, &aset);

    while (cnt < FD_SETSIZE) {
        rset = aset;

        int nready = select(max_fd + 1, &rset, NULL, NULL, NULL);
        if (nready == -1) {
            if (errno == EINTR) {
                continue;
            }
            printf("select run error.\n");
            break;
        }
        if (nready == 0) {
            continue;
        }
        if (FD_ISSET(sd, &rset)) {
            // Wait client's request, if request getted, return a new socket;
            // Server connect client with new socket.
            clients[cnt].ad = accept(sd, (struct sockaddr*)(&(clients[cnt].remote_ip)), &remote_len);
            if (clients[cnt].ad == -1) {
                printf("accept error, errno is %d\n", errno);
                continue;
            }
            
            // Find the min avalible position.
            int index = 0;
            for (; index < cnt; ++index) {
                if (clients[index].ad < 0) {
                    swap(clients[index], clients[cnt]);
                    break;
                }
            }

            clients[index].no = index + 1;
            nready -= 1;
            FD_SET(clients[index].ad, &aset);
            max_fd = max(max_fd, clients[index].ad);

            printf("------------client %d start!------------\n", index + 1);
            printf("ip is %s, port is %d.\n", inet_ntoa(clients[index].remote_ip.sin_addr), ntohs(clients[index].remote_ip.sin_port));
            cnt += (cnt == index);
        }

        // Here has other I/O socket to read data.
        for (int i = 0; i < cnt && nready > 0; ++i) {
            if (false == FD_ISSET(clients[i].ad, &rset)) {
                continue;
            }

            int conn = clients[i].ad;
            err = broadcast((void*)(&clients[i]));
            if (err == -1) {
                FD_CLR(conn, &aset);
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
