TCP(Transmission Control Protocol), UDP(User Datagram Protocol), SCTP(Stream Control Transmsion Protocol):
A. 1) UDP is a simple, unreliable, connectionless protocol.
   2) TCP is a complex, reliable, connection-base protocol.
   3) SCTP has some characters of above two protocols and give other characters TCP not available.
B. 1) TCP use three-way handshake to establish connection and use four-way handshake to abort connection.
   2) TCP connection has 11 states.
   3)We can use netstat -ni/r, ifconfig, ping commands to diagnose network problem.
C. TCP TIME_WAIT state is used to abort full duplex connection.
D. 1)SCTP use four-way handshake to establish connection and use three-way handshake to abort connection.
   2)SCTP connection has 8 states.

NOTICE:
A. Function memset has hidden problem(second/third param type is same), bzero has only two params.
B. 1) Thread function does not set Unix errno variable and regards value of errno as returned value when meets error.
   2) Socket funtion set Unix errno variable when meets error.
C. Funtion sprintf has buffer overflowing problem, use snprintf replaced.
D. Function gets, strcat, strcpy also buffer overflowing problem, use fgets, strncat, strncpy replaced separately.
E. Socket pair{ip1:port1, ip2:port2}.
