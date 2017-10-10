/*
*   1. int socket(int domain, int type, int protocol);
*   2. int connect(int sockfd, const struct sockaddr* addr, socklen_t addrlen)
*   3. read, write
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
#include <pthread.h>
#include <arpa/inet.h>

using namespace std;

#define MAX_LISTEN 10
#define BUFFER_SIZE 100
#define THREAD_NUM 5

int sd;
struct sockaddr_in server_ip;

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

void* thread_read(void* arg) {
    char buf[BUFFER_SIZE];
    int count;
    while (1) {
        memset(buf, 0, sizeof(buf));
        int size = readline(sd, buf, sizeof(buf));
        if (size <= 0) {
            continue;
        }

        printf("client 2 say: %s", buf);
    }
}

void* thread_write(void* arg) {
    char buf[BUFFER_SIZE];
    while (1) {
        memset(buf, 0, sizeof(buf));
        fgets(buf, sizeof(buf), stdin);

        writen(sd, buf, strlen(buf));
    }
}

int main() {
    int server_len;
    int remote_len;
    pthread_t tid_read;
    pthread_t tid_write;
    
    int err;
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
    
    // Connect to server.
    err = connect(sd, (struct sockaddr*)(&server_ip), sizeof(struct sockaddr));
    if (err == -1) {
        printf("connect error.\n");
        close(sd);
        return 0;
    }
    
    // Get sock name.
    struct sockaddr_in local_addr;
    socklen_t addr_len = sizeof(struct sockaddr);
    err = getsockname(sd, (struct sockaddr*)(&local_addr), &addr_len);
    if (err < 0) {
        printf("get host name failed.\n");
        close(sd);
        return 0;
    }
    printf("local ip: %s, port: %d\n", inet_ntoa(local_addr.sin_addr), ntohs(local_addr.sin_port));
    
    err = pthread_create(&tid_read, NULL, thread_read, NULL);
    if (err) {
        printf("create read thread failed.\n");
        close(sd);
        return 0;
    }
    err = pthread_create(&tid_write, NULL, thread_write, NULL);
    if (err) {
        printf("create write thread failed.\n");
        close(sd);
        pthread_join(tid_read, NULL);
        return 0;
    }
    
    pthread_join(tid_write, NULL);
    pthread_join(tid_read, NULL);
    
    close(sd);
    
    return 0;
}
