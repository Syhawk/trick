#ifndef INCLUDE_UNP_H
#define INCLUDE_UNP_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <math.h>
#include <algorithm>
#include <poll.h>

#include <sys/select.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/tcp.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <netinet/ip6.h>

#include <arpa/inet.h>

#define LISTENQ			1024
#define MAXLINE			4096
#define BUFFSIZE		8192
#define SERV_PORT		5678

#ifndef INFTIME
#define INFTIME -1
#endif	// INFTIME

#define SA struct sockaddr
typedef void Sigfunc(int);

union val {
    int i_val;
    long l_val;
    struct linger linger_val;
    struct timeval timeval_val;
};

#define HAVE_VSNPRINTF	1

#endif	// INCLUDE_UNP_H
