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

// For solving socket(TCP)'s sticky package problem, use fixed length packet.
struct Packet {
    int len;
    char buf[BUFFER_SIZE];
};

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

void* thread_read(void* arg) {
    struct Packet pack;
    int count;
    while (1) {
        memset(&pack, 0, sizeof(pack));
        int size = readn(sd, &(pack.len), sizeof(int));
        if (size < 4) {
            continue;
        }

        count = ntohl(pack.len);
        size = readn(sd, pack.buf, count);
        if (size < count) {
            continue;
        }

        printf("client 2 say: %s\n", pack.buf);
    }
}

void* thread_write(void* arg) {
    struct Packet pack;
    int count;
    while (1) {
        memset(&pack, 0, sizeof(pack));
        cin.getline(pack.buf, sizeof(pack.buf));
        count = strlen(pack.buf);
        if (count == 0) {
            continue;
        }
        pack.len = htonl(count);
        writen(sd, &pack, count + 4);
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
    
    err = connect(sd, (struct sockaddr*)(&server_ip), sizeof(struct sockaddr));
    if (err == -1) {
        printf("connect error.\n");
        close(sd);
        return 0;
    }
    
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
