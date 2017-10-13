/*
*   1. ssize_t read(int fd, void *buf, size_t count);
*   2. ssize_t write(int fd, const void *buf, size_t count);
*   3. int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
*   4. int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
*   5. int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
*   
*   6. readn, writen.
*   7. recv_peek, readline.
*   8. read_tiemout, write_timeout, accept_timeout, connect_timeout.
*   9. deactive_nonblock, deactive_nonblock.
*   10. getlocalip.
*/


#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <netdb.h>
#include <string.h>

#define ERR_EXIT(m) \
        do { \
            perror(m);    \
            exit(EXIT_FAILURE); \
        } while (0)

/*
*   readn: For solving problem buffer insufficient, implement new method by old method read.
*   @fd: File descriptor.
*   @buf: Read data to buf.
*   @count: Read data's maxsize.
*   return: success(>=0, read data size), fail(-1).
*/
ssize_t readn(int fd, void *buf, size_t count);

/*
*   writen: For solving problem buffer insufficient, implement new method by old method write.
*   @fd: File descriptor.
*   @buf: Write data from buf.
*   @count: Write data's maxsize.
*   return: success(>=0, write data size), fail(-1).
*/
ssize_t writen(int fd, const void *buf, size_t count);

/* 
*   recv_peek: For solving socket(TCP)'s sticky package problem, use special suffix('\n'),
*               read data, not clear buffer.
*   @sockfd: File descriptor.
*   @buf: Read data to buf.
*   @len: Read data's maxsize.
*   return: success(>=0, write data size), fail(-1).
*/
ssize_t recv_peek(int sockfd, void *buf, size_t len);

/* 
*   readline: For solving socket(TCP)'s sticky package problem, use special suffix('\n'),
*               read one line tailed by '\n'.
*   @sockfd: File descriptor.
*   @buf: Read data to buf.
*   @len: Read data's maxsize.
*   return: success(>=0, write data size), fail(-1).
*/
ssize_t readline(int sockfd, void *buf, size_t len);

/*
*   read_timeout: function of checking read timeout, exclude read action.
*   @fd: File descriptor.
*   @wait_seconds: Waiting time(second), if value is zero, not check timeout.
*   return: success (0), fail(-1, if checked timeout, errno is ETIMEDOUT).
*/
ssize_t read_timeout(int fd, unsigned int wait_seconds);

/*
*   write_timeout: function of checking write timeout, exclude write acton.
*   @fd: File descriptor.
*   @wait_seconds: Waiting time(second), if value is zero, not check timeout.
*   return: success(0), fail(-1, if checked timeout, errno is ETIMEDOUT).
*/
ssize_t write_timeout(int fd, unsigned int wait_seconds);

/*
*   accept_tiemout: Function of checking accept timeout.
*   @fd: File descriptor.
*   @addr: Output param, the other's address.
*   @wait_seconds: Waiting time(second), if value is zero, not check timeout.
*   return: success(socket descriptor), fail(-1, if checked timeout, error is ETIMEDOUT).
*/
ssize_t accept_timeout(int fd, struct sockaddr_in* addr, unsigned int wait_seconds);

/*
*   connect_timeout: Function of checking connect timeout.
*   @fd: File descriptor.
*   @addr: Connecting address.
*   @wait_seconds: Wait time(seconds), if value is zero, not check timeout.
*   return: success(0), fail(-1, if checked timeout, errno is ETIMEDOUT).
*/
ssize_t connect_timeout(int fd, struct sockaddr_in* addr, unsigned int wait_seconds);

/*
*   active_nonblock: Change I/O's mode to nonblock mode.
*   @fd: File descriptor.
*   return: success(0), fail(-1).
*/
ssize_t active_nonblock(int fd);

/*
*   deactive_nonblock: Change I/O's mode to block mode.
*   @fd: File descriptor.
*   return: success(0), fail(-1).
*/
ssize_t deactive_nonblock(int fd);

/*
*   getlocalip: Get local ip.
*   @ip: ip address.
*   return: success(0), fail(-1).
*/
ssize_t getlocalip(char* ip);

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

ssize_t read_timeout(int fd, unsigned int wait_seconds) {
    int ret = 0;
    if (wait_seconds > 0) {
        fd_set rset;
        struct timeval timeout;

        FD_ZERO(&rset);
        FD_SET(fd, &rset);

        timeout.tv_sec = wait_seconds;
        timeout.tv_usec = 0;

        do {
            ret = select(fd + 1, &rset, NULL, NULL, &timeout);
        } while (ret < 0 && errno == EINTR);

        if (ret == 0) {
            ret = -1;
            errno = ETIMEDOUT;
        } else if (ret == 1) {
            ret = 0;
        }
    }

    return ret;
}

ssize_t write_timeout(int fd, unsigned int wait_seconds) {
    int ret = 0;
    if (wait_seconds > 0) {
        fd_set wset;
        struct timeval timeout;
        
        FD_ZERO(&wset);
        FD_SET(fd, &wset);
        
        timeout.tv_sec = wait_seconds;
        timeout.tv_usec = 0;
        
        do {
        
        } while (ret < 0 && ret == EINTR);
        
        if (ret == 0) {
            ret = -1;
            errno = ETIMEDOUT;
        } else if (ret == 1) {
            ret = 0;
        }
    }
    
    return ret;
}

ssize_t accept_timeout(int fd, struct sockaddr_in* addr, unsigned int wait_seconds) {
    int ret = -1;
    socklen_t addr_len = sizeof(struct sockaddr_in);

    if (wait_seconds > 0) {
        fd_set accept_fdset;
        struct timeval timeout;
        
        FD_ZERO(&accept_fdset);
        FD_SET(fd, &accept_fdset);
        
        timeout.tv_sec = wait_seconds;
        timeout.tv_usec = 0;
        
        do {
            ret = select(fd + 1, &accept_fdset, NULL, NULL, &timeout);
        } while (ret == 0 && errno == EINTR);
        
        if (ret == -1) {
            return -1;
        }
        if (ret == 0) {
            errno = EINTR;
            return -1;
        }
    }

    if (addr != NULL) {
        ret = accept(fd, (struct sockaddr*)(addr), &addr_len);
    } else {
        ret = accept(fd, NULL, NULL);
    }

    return ret;
}

ssize_t active_nonblock(int fd) {
    int ret;
    int flgs = fcntl(fd, F_GETFL);
    if (flgs == -1) {
        return -1;
    }

    flgs |= O_NONBLOCK;
    ret = fcntl(fd, F_SETFL, flgs);
    if (ret == -1) {
        return -1;
    }

    return 0;
}

ssize_t deactive_nonblock(int fd) {
    int ret;
    int flgs = fcntl(fd, F_GETFL);
    if (flgs == -1) {
        return -1;
    }
    
    flgs &= ~O_NONBLOCK;
    ret = fcntl(fd, F_SETFL, flgs);
    if (ret == -1) {
        return -1;
    }

    return 0;
}

ssize_t connect_timeout(int fd, struct sockaddr_in* addr, unsigned int wait_seconds) {
    int ret;
    
    if (wait_seconds > 0) {
        active_nonblock(fd);
    }
    
    ret = connect(fd, (struct sockaddr*)addr, sizeof(struct sockaddr));
    if (ret < 0 && errno == EINPROGRESS) {
        fd_set connect_fdset;
        struct timeval timeout;
        
        FD_ZERO(&connect_fdset);
        FD_SET(fd, &connect_fdset);
        timeout.tv_sec = wait_seconds;
        timeout.tv_usec = 0;

        do {
            ret = select(fd + 1, NULL, &connect_fdset, NULL, &timeout);
        } while (ret < 0 && errno == EINTR);

        if (ret == 0) {
            errno = EINTR;
            return -1;
        }

        if (ret > 0) {
            // Ret's value is 1, here has two different condition. one: connect ok;
            // two: socket makes a error, but information can't be stored into errno, so, need to call getsockopt.
            int err;
            socklen_t sock_len = sizeof(err);
            ret = getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &sock_len);
            if (ret != -1 && err > 0) {
                ret = -1;
                errno = err;
            }
        }
    }

    if (wait_seconds > 0) {
        deactive_nonblock(fd);
    }

    return ret;
}

ssize_t getlocalip(char* ip) {
    // Get hostname.
    char host[100];
    if (gethostname(host, sizeof(host)) < 0) {
        perror("get host name failed");
        return -1;
    }
    printf("hostname is: %s\n", host);

    struct hostent* hep = NULL;
    if ((hep = gethostbyname(host)) == NULL) {
        perror("get host by name failed");
        return -1;
    }

    // Get all ip address.
    int i = 0;
    while (hep->h_addr_list[i] != NULL) {
        printf("%s\n", inet_ntoa(*((struct in_addr*)(hep->h_addr_list[i]))));
        ++i;
    }

    // Get ip address.
    strcpy(ip, inet_ntoa(*((struct in_addr*)hep->h_addr)));

    return 0;
}
