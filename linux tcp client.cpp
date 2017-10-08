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

void* thread_read(void* arg) {
    char buf[BUFFER_SIZE];
    while (1) {
        usleep(1);
        memset(buf, 0, sizeof(buf));
        size_t size = read(sd, buf, sizeof(buf));
        if (size == 0) {
            continue;
        }
        printf("client 2 say: %s\n", buf);
    }
}

void* thread_write(void* arg) {
    char buf[BUFFER_SIZE];
    while (1) {
        usleep(1);
        cin.getline(buf, sizeof(buf));
        if (strlen(buf) == 0) {
            continue;
        }
        write(sd, buf, sizeof(buf));
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
