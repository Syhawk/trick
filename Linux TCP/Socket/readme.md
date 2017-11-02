1. Transmit socket address struct from kernel to process has 5 socket funtion: accept, recvfrom, recvmsg, getpeername, getsockname.
2. Value-result param common example: select getsockopt, recvmsg, ifconf, sysctl etc.
3. Endian has two methods: Little-endian, Big-endian. OS use one of this, network use Big-endian.
4. Endian conversion function: htons, htonl, ntohs, ntohl.
5. Address conversion function: inet_aton, inet_addr(have hidden problem), inet_ntoa, inet_pton, inet_ntop.
6. Funtion read, write has hidden problem(errno is EINTR, need to continue call function) -> use readn, writen, readline instead(Implement by self).
7. AF_xxx indicate address family, PF_xxx indicate protocol family.
8. Function connect error return result analyze: errno is ETIMEDOUT; communication no be established(hard error); destination unreachable(soft error).
9. Function bind can specify port and ip address or not specify.
10. Function listen: convert a unconncted socket to passive socket; fix queue max connection num(include incomplete queue and completed connection queue).
11. Function listen param backlog has different means for diffenent os.
12. Function fork is called once and return twice: if returned value is 0, current process is child, more than 0 is father, -1 means failure.
13. Function fork has two typical usage mode: used by back-up to execute other task; execute another program.
14. Every file descriptor has a reference counter, you should remember to close it, particularly in calling fork.
