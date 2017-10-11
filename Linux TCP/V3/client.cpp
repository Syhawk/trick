/*
*   1. int socket(int domain, int type, int protocol).
*   2. int connect(int sockfd, const struct sockaddr* addr, socklen_t addrlen).
*   3. read, write, recv.
*   4. readn, writen: For solving problem buffer insufficient, implement new method by old method.
*   5. recv_peek, readline: For solving socket(TCP)'s sticky package problem, use special suffix('\n').
*   6. pthread_mutex, pthread_cond: Solve read I/O manipulate problem.
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
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t fd_sig;
pthread_cond_t sd_sig;

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

        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&sd_sig, &mutex);
        int size = readline(sd, buf, sizeof(buf));
        pthread_mutex_unlock(&mutex);

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
        
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&fd_sig, &mutex);
        fgets(buf, sizeof(buf), stdin);
        pthread_mutex_unlock(&mutex);
        
        writen(sd, buf, strlen(buf));
    }
}

void io_mointor() {
    fd_set rset;
    FD_ZERO(&rset);
    int fd_stdin = fileno(stdin);
    int max_fd = max(fd_stdin, sd) + 1;

    while (1) {
        FD_SET(fd_stdin, &rset);
        FD_SET(sd, &rset);

        int nready = select(max_fd, &rset, NULL, NULL, NULL);
        if (nready == -1) {
            printf("select io stdin failed.\n");
            return;
        }
        if (nready == 0) {
            continue;
        }

        if (FD_ISSET(fd_stdin, &rset)) {
            pthread_cond_signal(&fd_sig);
        }
        if (FD_ISSET(sd, &rset)) {
            pthread_cond_signal(&sd_sig);
        }
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
    
    pthread_cond_init(&fd_sig, NULL);
    pthread_cond_init(&sd_sig, NULL);
    
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

    io_mointor();

    pthread_join(tid_write, NULL);
    pthread_join(tid_read, NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&fd_sig);
    pthread_cond_destroy(&sd_sig);

    close(sd);

    return 0;
}
