/*
*   1. int socket(int domain, int type, int protocol);
*   2. int bind(int sockfd, const struct sockaddr* addr, socklen_t addrlen);
*   3. int listen(int sockfd, int backlog);
*   4. int accept(int sockfd, struct sockaddr* addr, socklen_t* addrlen);
*   5. read, write
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

struct Point {
    int ad;
    int no;
    sockaddr_in remote_ip;
    Point() {}
};

int cnt;
pthread_t tids[THREAD_NUM];
struct Point clients[THREAD_NUM];

void* thread_read(void* arg) {
    Point* p = (Point*)arg;
    char buf[BUFFER_SIZE];
    while (1) {
        memset(buf, 0, sizeof(buf));
        int size = read(p->ad, buf, sizeof(buf));
        if (size <= 0) {
            sleep(1);
            if (size < 0) {
                printf("------------client %d read data error!------------\n", p->no);
            }
            printf("------------client %d close!------------\n", p->no);
            close(p->ad);
            p->ad = -2;
            return (void*)1;
        }

        printf("read data from client: %s\n", inet_ntoa(p->remote_ip.sin_addr));
        printf("data is: %s\n", buf);

        for (int i = 0; i < cnt; ++i) {
            // Here has bug, when thread's number is big, ad's value is -2, it write message to this client.
            if (clients[i].ad != p->ad && clients[i].ad >= 0) {
                write(clients[i].ad, buf, sizeof(buf));
            }
        }
    }
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
    err = listen(sd, MAX_LISTEN);
    if (err == -1) {
        printf("listen error, errno is %d.\n", errno);
        close(sd);
        return 0;
    }

    // Get client ip address's size.
    remote_len = sizeof(struct sockaddr);
    // Use for to make sure client connecting to server.
    for (cnt = 0; cnt < THREAD_NUM; ++cnt) {
        // Wait client's request, if request getted, return a new socket;
        // Server connect client with new socket.
        clients[cnt].ad = accept(sd, (struct sockaddr*)(&(clients[cnt].remote_ip)), &remote_len);
        if (clients[cnt].ad == -1) {
            printf("accept error, errno is %d.\n", errno);
            continue;
        }
        clients[cnt].no = cnt + 1;

        printf("------------client %d start!------------\n", cnt + 1);
        printf("ip is %s, port is %d.\n", inet_ntoa(clients[cnt].remote_ip.sin_addr), ntohs(clients[cnt].remote_ip.sin_port));

        // Create a new thread for a new ad.
        err = pthread_create(&tids[cnt], NULL, thread_read, (void*)(&(clients[cnt])));
        if (err != 0) {
            printf("create new thread failed\n");
            close(clients[cnt].ad);
            clients[cnt].ad = -1;
        }
    }

    for (int i = 0; i < cnt; ++i) {
        if (clients[i].ad != -1) {
            pthread_join(tids[i], NULL);
        }
    }

    close(sd);

    return 0;
}
