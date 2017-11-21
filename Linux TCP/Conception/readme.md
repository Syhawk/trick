TCP(Transmission Control Protocol), UDP(User Datagram Protocol), SCTP(Stream Control Transmsion Protocol):
1. UDP is a simple, unreliable, connectionless protocol.
   TCP is a complex, reliable, connection-base protocol.
   SCTP has some characters of above two protocols and give other characters TCP not available.
2. TCP use three-way handshake to establish connection and use four-way handshake to abort connection.
   TCP connection has 11 states.
   We can use netstat -ni/r, ifconfig, ping commands to diagnose network problem.
3. TCP TIME_WAIT state is used to abort full duplex connection.
4. SCTP use four-way handshake to establish connection and use three-way handshake to abort connection.
   SCTP connection has 8 states.
5. 域名系统(Domain Name System,DNS)主要用于主机名字与IP地址之间的映射
    每个组织机构往往运行一个或多个名字服务器(Name Server)，
        即BIND(Berkeley Internet Name Domain)
    解析器就是主机名与IP地址之间的映射器
    解析器使用UDP查询的得到的结果超出了UDP消息的承载能力，自动切换到TCP
6. gethostbyname仅用于ipv4，尽量使用getaddrinfo(可支持ipv6)
    当发生错误时，不设置errno变量，设置全局变量h_error为<netdb.h>的取值之一：
    HOST_NOT_FOUND
    TRY_AGAIN
    NO_RECOVERY
    NO_DATA
7. gethostbyaddr函数试图由一个二进制的IP地址找到相应的主机名
8. getaddrinfo能够处理名字到地址以及服务到端口的转换
    1）如果hostname参数关联的地址有多个，每个地址都返回一个对应的结构
    2）如果service参数指定的服务支持多个套接字类型，那么返回多个对应的结构
    3）指定hostname和service是TCP或UDP客户进程调用getaddrinfo的常规输入
    4）典型的服务器进程只指定service，而不指定hostname，同时指定AI_PASSIVE标志
    5）支持IPv4和IPv6
9. gai_strerror，可以根据error类型返回对应的解释
10. freeaddrinfo，释放addrinfo类型的内存
11. 

NOTICE:
1. Function memset has hidden problem(second/third param type is same), bzero has only two params.
2. Thread function does not set Unix errno variable and regards value of errno as returned value when meets error.
   Socket funtion set Unix errno variable when meets error.
3. Funtion sprintf has buffer overflowing problem, use snprintf replaced.
4. Function gets, strcat, strcpy also buffer overflowing problem, use fgets, strncat, strncpy replaced separately.
5. Socket pair{ip1:port1, ip2:port2}.
