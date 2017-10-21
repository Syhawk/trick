/*
*   1. int socket(int domain, int type, int protocol);
*   2. int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
*   3. int listen(int sockfd, int backlog);
*   4. int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
*   5. ssize_t read(int fd, void *buf, size_t count);
*   6. ssize_t write(int fd, const void *buf, size_t count);
*   7. int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
*   8. readline, writen.
*
*   man 2 stat
*   man unix
*/

#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <sys/un.h>

#include "interface.h"
#include "common.h"

void echo_srv(int conn) {
    char buf[MSG_LEN];
    while (1) {
        memset(buf, 0, sizeof(buf));
        int err = readline(conn, buf, sizeof(buf));
        if (err == -1) {
            if (errno == EINTR) {
                continue;
            }
            ERR_EXIT("server read error");
        }
        if (err == 0) {
            printf("client close.\n");
            break;
        }

        fputs(buf, stdout);
        err = writen(conn, buf, strlen(buf));
        if (err == -1 && errno != EINTR) {
            ERR_EXIT("server write error.");
        }
        if (err == 0) {
            printf("client close.\n");
            break;
        }
    }
    close(conn);
}

int main() {
    // Create socket.
    int sock = socket(PF_UNIX, SOCK_STREAM, 0);
    if (sock == -1) {
        ERR_EXIT("server create socket error");
    }

    unlink("/tmp/test_sock");
    // Set family and path.
    struct sockaddr_un server_ip;
    memset(&server_ip, 0, sizeof(server_ip));
    server_ip.sun_family = AF_UNIX;
    strcpy(server_ip.sun_path, "/tmp/test_sock");

    // Bind ip address and port to socket.
    int err = bind(sock, (struct sockaddr*)(&server_ip), sizeof(struct sockaddr));
    if (err == -1) {
        close(sock);
        ERR_EXIT("server bind error");
    }

    // Listen.
    err = listen(sock, SOMAXCONN);
    if (err == -1) {
        close(sock);
        ERR_EXIT("server listen error");
    }

    int conn;
    pid_t pid;
    while (1) {
        conn = accept(sock, NULL, NULL);
        if (conn == -1) {
            if (conn == EINTR) {
                continue;
            }
            ERR_EXIT("server accept error");
        }
        
        pid = fork();
        if (pid == -1) {
            ERR_EXIT("fork error");
        }
        if (pid == 0) {
            close(sock);
            echo_srv(conn);
            exit(EXIT_SUCCESS);
        }
        close(conn);
    }

    return 0;
}
