TCP(Transmission Control Protocol), UDP(User Datagram Protocol), SCTP(Stream Control Transmsion Protocol):
1. 1.1 UDP is a simple, unreliable, connectionless protocol.
   1.2 TCP is a complex, reliable, connection-base protocol.
   1.3 SCTP has some characters of above two protocols and give other characters TCP not available.
2. 2.1 TCP use three-way handshake to establish connection and use four-way handshake to abort connection.
   2.2 TCP connection has 11 states.
   2.3 We can use netstat -ni/r, ifconfig, ping commands to diagnose network problem.
3. 3.1 TCP TIME_WAIT state is used to abort full duplex connection.
4. 4.1 SCTP use four-way handshake to establish connection and use three-way handshake to abort connection.
   4.2 SCTP connection has 8 states.

NOTICE:
1. Function memset has hidden problem(second/third param type is same), bzero has only two params.
2. 2.1 Thread function does not set Unix errno variable and regards value of errno as returned value when meets error.
   2.2 Socket funtion set Unix errno variable when meets error.
3. Funtion sprintf has buffer overflowing problem, use snprintf replaced.
4. Function gets, strcat, strcpy also buffer overflowing problem, use fgets, strncat, strncpy replaced separately.
5. Socket pair{ip1:port1, ip2:port2}.
