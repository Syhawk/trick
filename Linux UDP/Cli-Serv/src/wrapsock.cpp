#include "wrapsock.h"
#include "error.h"

char strres[128];
char ip[16];

ssize_t Read(int fildes, void *buf, size_t nbyte) {
	ssize_t nread = 0;
	char* ptr;

	ptr = (char*)buf;

	while (1) {
		if ((nread = read(fildes, ptr, nbyte)) < 0) {
			if (EINTR == errno) {
				continue;
			}
			return -1;
		}
		break;
	}

	return nread;
}

ssize_t Readn(int fildes, void* buf, size_t nbyte) {
	size_t nleft;
	ssize_t nread;
	char* ptr;

	nleft = nbyte;
	ptr = (char*)buf;

	while (nleft > 0) {
		if ((nread = read(fildes, ptr, nleft)) < 0) {
			if (EINTR == errno) {
				continue;
			}
			return -1;
		}
		if (nread == 0) {
			break;
		}

		nleft -= nread;
		ptr += nread;
	}

	return nbyte - nleft;
}

ssize_t Write(int fildes, const void *buf, size_t nbyte) {
	ssize_t nwrite = 0;
	char* ptr;

	ptr = (char*)buf;

	while (1) {
		if ((nwrite = write(fildes, ptr, nbyte)) < 0) {
			if (EINTR == errno) {
				continue;
			}
			return -1;
		}
		break;
	}

	return nwrite;
}

ssize_t Writen(int fildes, const void *buf, size_t nbyte) {
	size_t nleft;
	ssize_t nwrite;
	const char* ptr;

	nleft = nbyte;
	ptr = (char*)buf;

	while (nleft > 0) {
		if ((nwrite = write(fildes, ptr, nleft)) < 0) {
			if (EINTR == errno) {
				continue;
			}
			return -1;
		}

		if (nwrite == 0) {
			break;
		}

		nleft -= nwrite;
		ptr += nwrite;
	}

	return nbyte - nleft;
}

ssize_t Sendto(int sockfd, const void *buf, size_t len, int flags,
        const struct sockaddr *dest_addr, socklen_t addrlen) {
    size_t nleft;
    ssize_t nwrite;
    const char* ptr;

    nleft = len;
    ptr = (char*)buf;

    while (nleft > 0) {
        if ((nwrite = sendto(sockfd, ptr, nleft, flags, dest_addr, addrlen)) < 0) {
            if (EINTR == errno) {
                continue;
            }
            return -1;
        }

        if (nwrite == 0) {
            break;
        }

        nleft -= nwrite;
        ptr += nwrite;
    }

    return len - nleft;
}

ssize_t Recvfrom(int sockfd, void *buf, size_t len, int flags,
        struct sockaddr *src_addr, socklen_t *addrlen) {
    ssize_t nread;

    while (1) {
        if ((nread = recvfrom(sockfd, buf, len, flags, src_addr, addrlen)) < 0) {
            if (EINTR == errno) {
                continue;
            }
            return -1;
        }

        break;
    }

    return nread;
}

int Socket(int domain, int type, int protocol) {
	int sockfd;
	sockfd = socket(domain, type, protocol);
	if (sockfd == -1) {
		err_sys("socket error");
	}

	return sockfd;
}

void Setsockresume(int sockfd) {
	int on = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
		close(sockfd);
		err_sys("setsockopt error");
	}

	return;
}

void Bind(int sockfd, const struct sockaddr* addr, socklen_t addrlen) {
	if (bind(sockfd, addr, addrlen) < 0) {
		err_sys("bind error");
	}

	return;
}

void Listen(int fd, int backlog) {
	char* ptr;
	if ((ptr = getenv("LISTENNQ")) != NULL) {
		backlog = atoi(ptr);
	}

	if (listen(fd, backlog) < 0) {
		err_sys("listen error");
	}

	return;
}

int Accept(int socket, struct sockaddr* address, socklen_t* address_len) {
	int sockfd;
	while (1) {
		sockfd = accept(socket, address, address_len);
		if (sockfd == -1) {
			if (EINTR == errno) {
				continue;
			}
			err_sys("accept error");
		}
		break;
	}

	return sockfd;
}

void Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
	if (connect(sockfd, addr, addrlen) == -1) {
		if (EINTR == errno) {
			err_sys("connect error with errno is EINTR");
		} else {
			err_sys("connect error");
		}
	}

	return;
}

void Close(int fd) {
	while (1) {
		if (close(fd) < 0) {
			if (EINTR == errno) {
				continue;
			}
			err_sys("close error");
		}
		break;
	}

	return;
}

void Shutdown(int socket, int how) {
	if (shutdown(socket, how) < 0) {
		err_sys("shutdown error");
	}

	return;
}

int Select(int nfds, fd_set *readfds, fd_set *writefds,
		fd_set *exceptfds, struct timeval *timeout) {
	int nready;
	while (1) {
		if ((nready = select(nfds, readfds, writefds, exceptfds, timeout)) < 0) {
			if (EINTR == errno) {
				continue;
			}
			err_sys("select error");
		}
		break;
	}

	return nready;
}

int Poll(struct pollfd *fds, nfds_t nfds, int timeout) {
	int nready;
	while (1) {
		if ((nready = poll(fds, nfds, timeout)) < 0) {
			if (EINTR == errno) {
				continue;
			}
			err_sys("poll error");
		}
		break;
	}

	return nready;
}

Sigfunc* Signal(int signo, Sigfunc* func) {
	struct sigaction act;
	struct sigaction oact;
	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	if (signo == SIGALRM) {
#ifdef SA_INTERRUPT
		act.sa_flags |= SA_INTERRUPT;
#endif
	} else {
#ifdef SA_RESTART
	act.sa_flags |= SA_RESTART;
#endif
	}
 
	if (sigaction(signo, &act, &oact) < 0) {
		return (SIG_ERR);
	}

	return (oact.sa_handler);
}

void sig_chld(int signo) {
	pid_t pid;
	int stat;
	while ((pid = waitpid(-1, &stat, WNOHANG)) > 0) {
		printf("*child %d terminated\n", pid);
	}

	return;
}

void tcp_serv_echo(int sockfd) {
	char buf[MAXLINE] = {0};

	while (1) {
		ssize_t n = read(sockfd, buf, MAXLINE);
		if (n < 0) {
			err_sys("serv_echo: read error");
		}
		if (n == 0) {
			printf("client close\n");
			break;
		}
		fputs(buf, stdout);
	
		Writen(sockfd, buf, n);
		bzero(buf, sizeof(buf));
	}

	return;
}

void tcp_cli_echo(int sockfd) {
	char sendline[MAXLINE] = {0};
	char recvline[MAXLINE] = {0};

	while (fgets(sendline, MAXLINE, stdin) != NULL) {
		int n = Writen(sockfd, sendline, strlen(sendline));
        Readn(sockfd, recvline, n);
		if ( n <= 0) {
			err_quit("cli_echo: server terminated prematurely");
		}

		fputs(recvline, stdout);

		bzero(sendline, sizeof(sendline));
		bzero(recvline, sizeof(recvline));
	}

	return;
}

void tcp_cli_select_echo(int sockfd) {
	char sendline[MAXLINE] = {0};
	char recvline[MAXLINE] = {0};
	fd_set rset;
	int fd = fileno(stdin);
	int stdineof = 0;
	int maxfd = std::max(fd, sockfd);

	FD_ZERO(&rset);

	while (1) {
		if (stdineof == 0) {
			FD_SET(fd, &rset);
		}
		FD_SET(sockfd, &rset);

		Select(maxfd + 1, &rset, NULL, NULL, NULL);

		if (FD_ISSET(sockfd, &rset)) {
			int nread = Read(sockfd, recvline, sizeof(recvline));
			if (nread == 0) {
				if (stdineof == 1) {
					return;
				} else {
					err_quit("cli_echo: server terminated prematurely");
				}
			}
			fputs(recvline, stdout);
			bzero(recvline, sizeof(recvline));
		}
		if (FD_ISSET(fd, &rset)) {
			int nread = Read(fd, sendline, sizeof(sendline));
			if (nread <= 0) {
				stdineof = 1;
				Shutdown(sockfd, SHUT_WR);
				FD_CLR(fd, &rset);
				continue;
			}
			Writen(sockfd, sendline, nread);
			bzero(sendline, sizeof(sendline));
		}
	}

    return;
}

void udp_serv_echo(int sockfd) {
    char buf[MAXLINE] = {0};
    struct sockaddr_in addr;
    socklen_t len;

    while (1) {
        len = sizeof(addr);
        int nread = Recvfrom(sockfd, buf, sizeof(buf), 0, (SA*)&addr, &len);

        printf("buf is %s", buf);

        Sendto(sockfd, buf, nread, 0, (SA*)&addr, len);

        bzero(buf, sizeof(buf));
    }

    return;
}

void udp_cli_echo(int sockfd, SA* servaddr, socklen_t servlen) {
    char sendbuf[MAXLINE] = {0};
    char recvbuf[MAXLINE] = {0};
    struct sockaddr* replay_addr;
    socklen_t len;

    replay_addr = (struct sockaddr*)Malloc(servlen);

    // Set RCVBUF size.
    size_t size = 220 * 1024;
    Setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));

    // Connect with udp.
    Connect(sockfd, servaddr, servlen);

    while (fgets(sendbuf, sizeof(sendbuf), stdin) != NULL) {
        Sendto(sockfd, sendbuf, strlen(sendbuf), 0, servaddr, servlen);

        len = servlen;
        int nread = Recvfrom(sockfd, recvbuf, sizeof(recvbuf), 0, replay_addr, &len);
/*
 * // for other machine ip address.
        if (len != servlen || memcmp(servaddr, replay_addr, len) != 0) {
            printf("replay from %s (ignored)\n", Sock_ntop(replay_addr));
            printf("ip is %s\n", Sock_ntop(servaddr));
            continue;
        }
*/
        fputs(recvbuf, stdout);
        printf("recvbuf size = %d\n", nread);

        bzero(sendbuf, sizeof(sendbuf));
        bzero(recvbuf, sizeof(recvbuf));
    }
    
    Free(replay_addr);

    return;
}

char* sock_str_flag(union val* ptr, int len) {
    bzero(strres, sizeof(strres));

    if (len != sizeof(int)) {
        snprintf(strres, sizeof(strres), "size (%d) not sizeof(int)", len);
    } else {
        snprintf(strres, sizeof(strres), "%s", ptr->i_val == 0 ? "off" : "on");
    }

    return strres;
}

char* sock_str_int(union val* ptr, int len) {
    bzero(strres, sizeof(strres));

    if (len != sizeof(int)) {
        snprintf(strres, sizeof(strres), "size (%d) not sizeof(int)", len);
    } else {
        snprintf(strres, sizeof(strres), "%d", ptr->i_val);
    }

    return strres;
}

char* sock_str_linger(union val* ptr, int len) {
    bzero(strres, sizeof(strres));

    if (len != sizeof(struct linger)) {
        snprintf(strres, sizeof(strres), "size (%d) not sizeof(struct linger)", len);
    } else {
        snprintf(strres, sizeof(strres), "l_onoff = %d, l_linger = %d",
                ptr->linger_val.l_onoff, ptr->linger_val.l_linger);
    }

    return strres;
}

char* sock_str_timeval(union val* ptr, int len) {
    bzero(strres, sizeof(strres));

    if (len != sizeof(struct timeval)) {
        snprintf(strres, sizeof(strres), "size (%d) not sizeof(struct timeval)", len);
    } else {
        snprintf(strres, sizeof(strres), "%ld sec, %ld usec",
                ptr->timeval_val.tv_sec, ptr->timeval_val.tv_usec);
    }

    return strres;
}

void *Malloc(size_t size) {
    if (size == 0) {
        printf("malloc size should greater then 0.\n");
        return NULL;
    }

    return malloc(size);
}

void Free(void* ptr) {
    if (NULL == ptr) {
        printf("free pointer should not be NULL.\n");
    }

    free(ptr);

    return;
}

char* Sock_ntop(SA* addr) {
    bzero(ip, sizeof(ip));
    strncpy(ip, inet_ntoa(((struct sockaddr_in*)addr)->sin_addr), sizeof(ip));

    return ip;
}

int Setsockopt(int sockfd, int level, int optname,
        const void *optval, socklen_t optlen) {
    return setsockopt(sockfd, level, optname, optval, optlen);
}

pid_t Fork(void) {
    return fork();
}
