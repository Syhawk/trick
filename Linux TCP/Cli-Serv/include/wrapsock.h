#ifndef INCLUDE_WRAPSOCK_H
#define INCLUDE_WRAPSOCK_H

#include "unp.h"

ssize_t Read(int fildes, void *buf, size_t nbyte);
ssize_t Readn(int fildes, void* buf, size_t nbyte);
ssize_t Write(int fildes, const void *buf, size_t nbyte);
ssize_t Writen(int fildes, const void* buf, size_t nbyte);

int Socket(int domain, int type, int protocol);
void Setsockresume(int sockfd);
void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
void Listen(int fd, int backlog);
int Accept(int socket, struct sockaddr * address, socklen_t * address_len);
void Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

void Close(int fd);
void Shutdown(int socket, int how);

int Select(int nfds, fd_set *readfds, fd_set *writefds,
		fd_set *exceptfds, struct timeval *timeout);
int Poll(struct pollfd *fds, nfds_t nfds, int timeout);

Sigfunc* Signal(int signo, Sigfunc* func);
void sig_chld(int signo);

void serv_echo(int sockfd);
void cli_echo(int sockfd);
void cli_select_echo(int sockfd);

#endif	// INCLUDE_WRAPSOCK_H
