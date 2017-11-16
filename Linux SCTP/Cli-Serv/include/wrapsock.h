#ifndef INCLUDE_WRAPSOCK_H
#define INCLUDE_WRAPSOCK_H

#include "unp.h"

ssize_t Read(int fildes, void *buf, size_t nbyte);
ssize_t Readn(int fildes, void* buf, size_t nbyte);
ssize_t Write(int fildes, const void *buf, size_t nbyte);
ssize_t Writen(int fildes, const void* buf, size_t nbyte);
ssize_t Sendto(int sockfd, const void *buf, size_t len, int flags,
        const struct sockaddr *dest_addr, socklen_t addrlen);
ssize_t Recvfrom(int sockfd, void *buf, size_t len, int flags,
        struct sockaddr *src_addr, socklen_t *addrlen);

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

void Tcp_serv_echo(int sockfd);
void Tcp_cli_echo(int sockfd);
void Tcp_cli_select_echo(int sockfd);

void Udp_serv_echo(int sockfd);
void Udp_cli_echo(int sockfd, SA* servaddr, socklen_t servlen);

void Sctp_cli_echo(int sockfd, SA* servaddr, socklen_t servlen);
void Sctp_cli_echoall(int sockfd, SA* servaddr, socklen_t servlen);

int Sctp_sendmsg(int sd, const void * msg, size_t len,
        struct sockaddr *to, socklen_t tolen, uint32_t ppid, uint32_t flags,
        uint16_t stream_no, uint32_t timetolive, uint32_t context);

int Sctp_recvmsg(int sd, void * msg, size_t len,
        struct sockaddr * from, socklen_t * fromlen,
        struct sctp_sndrcvinfo * sinfo, int * msg_flags);

char* Sock_str_flag(union val* ptr, int len);
char* Sock_str_int(union val* ptr, int len);
char* Sock_str_linger(union val* ptr, int len);
char* Sock_str_timeval(union val* ptr, int len);

void* Malloc(size_t size);
void Free(void* ptr);

char* Sock_ntop(SA* addr);

int Setsockopt(int sockfd, int level, int optname,
        const void *optval, socklen_t optlen);

pid_t Fork(void);

#endif	// INCLUDE_WRAPSOCK_H
