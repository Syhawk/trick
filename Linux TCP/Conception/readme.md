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
11. getnameinfo是与getaddrinfo互补的函数
    1）NI_NUMERICHOST标志告知getnameinfo不要调用DNS，而是数值表达格式以字符串形式返回
    2）gethostbyname、gethostbyaddr、getservbyname和getservbyport这四个函数是不可重入的，
        它们都指向同一个静态结构指针
    3）inet_pton和inet_ntop可重入
    4）inet_ntoa不可重入
    5）getaddrinfo可重入的前提是由它调用的函数都可重入，即gethostbyname和getservbyname均可重入
    6）getnameinfo可重入的前提是由它调用的函数都可重入，即gethostbyaddr和getservbyport均可重入
12. 可重入函数改写
    1）把由不可重入函数填写并返回静态结构的做法改为由调用者分配，再由可重入函数填写的结构
        这会添加更多的指针参数以及其他参数，比较复杂
    2）可重入函数调用malloc动态分配内存空间
        必须调用释放动态内存的函数，如果进程长时间运行，内存消耗持续增加
13. IPv4客户与IPv6服务器
    1）IPv6服务器自动启动后创建一个IPv6的监听套接字，服务器把通配地址捆绑到该套接字
    2）IPv4客户调用gethostbyname找到服务器主机的一个A记录，服务器主机同时支持IPv4和IPv6
    3）客户调用connect，导致客户主机发送一个IPv4 SYN到服务器
    4）服务器主机接收这个目的地址为IPv6监听套接字的IPv4 SYN，设置一个标识指示本连接使用
        IPv4地址，然后响应一个IPv4的SYN/ACK
    5）当服务器主机往这个IPv4映射的IPv6地址发送TCP分节时，其IP栈产生目的地址为所映射
        IPv4地址的IPv4载送数据报
    6）除非服务器显示检查这个IPv6地址是不是一个IPv4映射的IPv6地址，否则永远不会知道是在
        与一个IPv4客户通信
14. IPv6客户与IPv4服务器
    1）一个IPv4服务器在只支持IPv4的一个主机上启动后创建一个IPv4的监听套接字
    2）IPv6客户启动后调用getaddrinfo单纯查找IPv6地址
    3）IPv6客户在作为函数参数的IPv6套接字地址结构中设置这个IPv4映射的IPv6地址后调用connect，
        内核检测到这个映射地址后自动发送一个IPv4 SYN到服务器
    4）服务器响应一个IPv4 SYN/ACK，连接于是通过使用IPv4数据报建立
15. 源代码可移植性
    1）添加#ifdef伪代码，但代码维护和理解比较困难
    2）把IPv4转向为IPv6变成协议无关
16. 守护进程
    守护进程(daemon)是在后台运行且不与任何控制终端关联的进程
    启动方法：
    1）由系统初始化脚本启动
    2）由inetd超级服务器启动
    3）cron守护进程按照规则定期执行一些程序，由它启动执行的程序同样作为守护进程运行
    4）at命令用于指定将来某个时刻的程序执行
    5）守护进程还可以从用户终端或在前台或在后台启动
17. inetd守护进程
    传统守护进程问题：
    1）含有几乎相同的代码，既表现在创建套接字上，也表现在演变成守护进程上
    2）每个守护进程在进程表中占据一个表项，大部分时间处于睡眠状态
    解决方法：
    1）inetd处理普通守护进程的大部分启动细节以简化守护程序的编写
    2）单个进程（inetd）就能为多个服务等待外来的客户请求
    注：
    inetd不关闭对于某个数据报套接字的可读条件检查，而且父进程先于服务该套接字的子进程执行，
    那么引发本次的那个数据报仍然在套接字接受缓冲区中，导致slect再次返回可读条件，致使inetd再次fork另一个子进程

NOTICE:
1. Function memset has hidden problem(second/third param type is same), bzero has only two params.
2. Thread function does not set Unix errno variable and regards value of errno as returned value when meets error.
   Socket funtion set Unix errno variable when meets error.
3. Funtion sprintf has buffer overflowing problem, use snprintf replaced.
4. Function gets, strcat, strcpy also buffer overflowing problem, use fgets, strncat, strncpy replaced separately.
5. Socket pair{ip1:port1, ip2:port2}.
