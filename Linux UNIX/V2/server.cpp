/*
*   1. int socket(int domain, int type, int protocol);
*   2. int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
*   3. int listen(int sockfd, int backlog);
*   4. int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
*   5. ssize_t read(int fd, void *buf, size_t count);
*   6. ssize_t write(int fd, const void *buf, size_t count);
*   7. int socketpair(int domain, int type, int protocol, int sv[2]);
*   8. ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags);
*   9. ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags);
*   10. readline, writen.
*
*   man 2 stat
*   man unix
*   man CMSG_FIRSTHDR
*   pipe use to relationship communication, full duplex.
*/

#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <sys/un.h>

#include "interface.h"
#include "common.h"

void send_sockfd(int sock, int send_fd) {
    struct msghdr msg;

    // Set msg name.
    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    // Set msg content.
    char sendchar = '0';
    struct iovec vec;
    vec.iov_base = &sendchar;
    vec.iov_len = sizeof(sendchar);
    msg.msg_iov = &vec;
    msg.msg_iovlen = 1;
    // Set msg control.
    char cmsgbuf[CMSG_SPACE(sizeof(send_fd))];
    msg.msg_control = cmsgbuf;
    msg.msg_controllen = sizeof(cmsgbuf);
    struct cmsghdr* p_cmsg = CMSG_FIRSTHDR(&msg);
    p_cmsg->cmsg_level = SOL_SOCKET;
    p_cmsg->cmsg_type = SCM_RIGHTS;
    p_cmsg->cmsg_len = CMSG_LEN(sizeof(send_fd));
    int* p_fd = (int*)CMSG_DATA(p_cmsg);
    *p_fd = send_fd;
    // Set msg flags.
    msg.msg_flags = 0;

    int ret = sendmsg(sock, &msg, 0);
    if (ret == -1) {
        ERR_EXIT("sendmsg error");
    }
}

int recv_sockfd(const int sock) {
    struct msghdr msg;

    // Set msg name.
    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    // Set msg content.
    char recvchar;
    struct iovec vec;
    vec.iov_base = &recvchar;
    vec.iov_len = sizeof(recvchar);
    msg.msg_iov = &vec;
    msg.msg_iovlen = 1;
    // Set msg control.
    int recv_fd;
    char cmsgbuf[CMSG_SPACE(sizeof(recv_fd))];
    msg.msg_control = cmsgbuf;
    msg.msg_controllen = sizeof(cmsgbuf);
    int* p_fd = (int*)CMSG_DATA(CMSG_FIRSTHDR(&msg));
    *p_fd = -1;
    // Set msg flags.
    msg.msg_flags = 0;

    int ret = recvmsg(sock, &msg, 0);
    if (ret == -1) {
        ERR_EXIT("recvmsg error");
    }

    struct cmsghdr* p_cmsg = CMSG_FIRSTHDR(&msg);
    if (p_cmsg == NULL) {
        ERR_EXIT("msg is NULL");
    }

    p_fd = (int*)CMSG_DATA(p_cmsg);
    recv_fd = *p_fd;
    if (recv_fd == -1) {
        ERR_EXIT("no passed fd");
    }

    return recv_fd;
}

int main() {
    // Create socket.
    int sockfds[2];
    int err = socketpair(PF_UNIX, SOCK_STREAM, 0, sockfds);
    if (err == -1) {
        ERR_EXIT("server create socket error");
    }

    pid_t pid;
    pid = fork();
    if (pid == -1) {
        ERR_EXIT("fork");
    }
    if (pid > 0) {
        close(sockfds[1]);
        int fd = recv_sockfd(sockfds[0]);
        char buf[1024] = {0};
        read(fd, buf, sizeof(buf));
        printf("buf = %s\n", buf);
    } else {
        close(sockfds[0]);
        int fd;
        fd = open("test.txt", O_RDONLY);
        if (fd == -1) {
            ERR_EXIT("open file error");
        }
        send_sockfd(sockfds[1], fd);
    }
/*
    if (pid > 0) {
        close(sockfds[1]);
        int val = 0;
        while (1) {
        
            ++val;
            printf("send data: %d\n", val);
            write(sockfds[0], &val, sizeof(val));
            read(sockfds[0], &val, sizeof(val));
            printf("data recved: %d\n", val);
            sleep(1);
        }
    } else {
        int val;
        close(sockfds[0]);
        while (1) {
            read(sockfds[1], &val, sizeof(val));
            ++val;
            write(sockfds[1], &val, sizeof(val));
        }
    }
*/
    return 0;
}
