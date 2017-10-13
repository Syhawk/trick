/*
*   1. int socket(int domain, int type, int protocol).
*   2. int connect(int sockfd, const struct sockaddr* addr, socklen_t addrlen).
*   3. pthread_mutex, pthread_cond.
*   4. ssize_t readn(int fd, void *buf, size_t count);
*   5. ssize_t writen(int fd, const void *buf, size_t count);
*   6. ssize_t readline(int sockfd, void *buf, size_t len);
*   7. int shutdown(int sockfd, int how);
*   8. int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
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
#include <pthread.h>
#include <arpa/inet.h>
#include "interface.h"

using namespace std;

#define MAX_LISTEN 10
#define BUFFER_SIZE 100
#define THREAD_NUM 5

int close_flg;
int sd;
struct sockaddr_in server_ip;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t fd_sig;
pthread_cond_t sd_sig;

// pthread_mutex, pthread_cond: Solve read I/O manipulate problem.
void* thread_read(void* arg) {
    char buf[BUFFER_SIZE];
    int count;

    while (!close_flg) {
        memset(buf, 0, sizeof(buf));

        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&sd_sig, &mutex);
        int size = readline(sd, buf, sizeof(buf));
        pthread_mutex_unlock(&mutex);

        if (size <= 0) {
            printf("server closed.\n");
            close_flg = true;
            return (void*)1;
        }
        printf("client 2 say: %s", buf);
    }
}

void* thread_write(void* arg) {
    char buf[BUFFER_SIZE];

    while (!close_flg) {
        memset(buf, 0, sizeof(buf));
        
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&fd_sig, &mutex);
        char* rd = NULL;
        if (!close_flg) {
            rd = fgets(buf, sizeof(buf), stdin);
        }
        pthread_mutex_unlock(&mutex);

        if (rd == NULL) {
            if (close_flg) {
                close(sd);
            } else {
                shutdown(sd, SHUT_WR);
            }
            return (void*)1;
        }
        writen(sd, buf, strlen(buf));
    }
}

void io_mointor() {
    fd_set rset;
    FD_ZERO(&rset);
    int fd_stdin = fileno(stdin);
    int max_fd = max(fd_stdin, sd) + 1;

    while (!close_flg) {
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

        if (FD_ISSET(sd, &rset)) {
            pthread_cond_signal(&sd_sig);
        }

        if (FD_ISSET(fd_stdin, &rset) || close_flg) {
            pthread_cond_signal(&fd_sig);
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
        ERR_EXIT("create socket error");
    }

    // Set ip address and port.
    server_ip.sin_family = AF_INET;
    server_ip.sin_port = htons(5678);
    server_ip.sin_addr.s_addr = htonl(INADDR_ANY);
    memset(server_ip.sin_zero, 0, sizeof(server_ip.sin_zero));

    // Connect to server.
    // err = connect(sd, (struct sockaddr*)(&server_ip), sizeof(struct sockaddr));
    unsigned int wait_seconds = 3;
    err = connect_timeout(sd, &server_ip, wait_seconds);
    if (err == -1) {
        close(sd);
        if (errno == ETIMEDOUT) {
            ERR_EXIT("connect timeout...");
        } else {
            ERR_EXIT("connect error");
        }
    }

    // Get sock name.
    struct sockaddr_in local_addr;
    socklen_t addr_len = sizeof(struct sockaddr);
    err = getsockname(sd, (struct sockaddr*)(&local_addr), &addr_len);
    if (err < 0) {
        close(sd);
        ERR_EXIT("getsockname error");
    }
    printf("local ip: %s, port: %d\n", inet_ntoa(local_addr.sin_addr), ntohs(local_addr.sin_port));

    pthread_cond_init(&fd_sig, NULL);
    pthread_cond_init(&sd_sig, NULL);
    close_flg = false;

    err = pthread_create(&tid_read, NULL, thread_read, NULL);
    if (err) {
        close(sd);
        ERR_EXIT("create read thread failed");
    }
    err = pthread_create(&tid_write, NULL, thread_write, NULL);
    if (err) {
        close(sd);
        ERR_EXIT("create write thread failed");
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
