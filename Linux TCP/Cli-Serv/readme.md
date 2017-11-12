1. Transmit socket address struct from kernel to process has 5 socket funtion:
    accept, recvfrom, recvmsg, getpeername, getsockname.
2. Value-result param common example: select getsockopt, recvmsg, ifconf, sysctl etc.
3. Endian has two methods: Little-endian, Big-endian.
    OS use one of this, network use Big-endian.
4. Endian conversion function: htons, htonl, ntohs, ntohl.
5. Address conversion function:
    inet_aton, inet_addr(have hidden problem), inet_ntoa, inet_pton, inet_ntop.
6. Funtion read, write has hidden problem(errno is EINTR, need to continue call function)
    -> use readn, writen, readline instead(Implement by self).
7. AF_xxx indicate address family, PF_xxx indicate protocol family.
8. connect error return result analyze: 
    1) errno is ETIMEDOUT
    2) communication no be established(hard error)
    3) destination unreachable(soft error)
9. bind
    can specify port and ip address or not specify.
10. listen
    1) convert a unconncted socket to passive socket
    2) fix queue max connection num(include incomplete queue and completed connection queue)
    3) param backlog has different means for diffenent os
11. fork
    return value:
    1) returned value is 0, current process is child
    2) returned value more than 0, current process is father
    3) returned value is -1 means failure
    typical usage mode:
    1) used by back-up to execute other task
    2) execute another program
12. file descriptor
    It has a reference counter, you should remember to close it, particularly in calling fork
13. ps -t pts/t -o pid,ppid,tty,stat,args,wchan.
14. 信号(signal)就是告知某个进程发生了某个事件的通知，有时也称为软件中断(software interrupt)
    信号通常是异步发生的，也就是说进程预先不知道信号的准确发生时刻。
    信号可以：
    1）由一个进程发给另一个进程
    2）由内核发给某个进程
    注：
    1）有两个信号不能被捕获：SIGKILL和SIGSTOP，而且这两个信号也不能被忽略
    2）SIGIO，SIGPOLL，SIGURG这些个别信号要求捕获它们的进程做些额外的工作
    3）可以把某个信号的处置设定为SIG_DEL来启动它的默认处置。默认处置通常是在收到信号后终止进程,
        其中某些信号还在当前工作目录产生一个进程的核心影响
    4）被中断的系统调用不一定会自动重启，需要额外注意(观察errno的值是否为EINTR)
    5）信号在被阻塞期间产生了一次或多次，信号被解阻塞后通常只递交一次，也就是说Unix信号默认是不排队的
    6）利用sigprocmask函数选择性的阻塞或者解阻塞一组信号时可能的。
        这使得我们可以做到在一段临界区代码执行期间，防止捕获某些信号，以保护这段代码
15. 僵死进程(zombie)
    1) 设置僵死状态的目的是维护子进程的信息，以便父进程在以后某个时候获取，这些信息包括:
        子进程的进程id，终止状态以及资源利用信息(CPU时间、内存使用量等等)。
    2) 如果一个进程终止，而该进程有子进程处于僵死状态，那么它的所有僵死子进程的父进程id将被重置
        为1（init进程），继承这些子进程的init进程将清理它们
16. connect因为中断返回EINTR时，不能够再次调用它，否则立即返回一个错误，必须调用select来等待连接完成
17. wait与waitpid
    1）调用wait时，wait会阻塞到现在子进程第一个终止为止
    2）调用waitpid可以阻塞到没有子进程运行为止
18. fork
    1）fork子进程时，必须捕获SIGCHLD信号
    2）捕获信号时，必须处理被中断的系统调用
    3）SIGHCLD的信号处理函数必须正确编写，应使用waitpid函数以免留下僵死进程
19. accept
    流子系统（stream connection abort）中发生某些致命的协议相关事件时，也会返回EPROTO
    要是对于由于客户引起的一个已建立链接的非致命终止也返回同样的错误，
    那么服务器就不知道是否再次调用accept，换成ECONNABORTED错误，服务器就可以忽略它，再次调用accept
20. SIGPIPE
    服务器终止后，客户端第一次写操作引起RST，第二次写操作引发SIGPIPE信号，该信号的默认行为是终止进程,
    因此进程必须捕获它，以免被终止
21. 服务器主机关机
    Unix系统关机时，init进程通常先给所有进程发送SIGTERM信号，然后再发送SIGKILL信号。
    如果不捕获SIGTREM信号并终止，服务器将由SIGKILL终止
22. 非阻塞式I/O模型
    1）I/O复用模型
        采用select或者poll，阻塞在这两个系统调用之上，而不是阻塞在真正的I/O系统调用上
    2）信号驱动式I/O模型
        让内核在描述符就绪时发送SIGIO信号通知我们
    3）异步I/O模型
        让内核通知我们I/O操作何时完成
23. 同步异步I/O对比
    1）同步I/O操作(synchronous I/O operation)：导致请求进程阻塞，直到I/O操作完成
    2）异步I/O操作(asynchronous I/O operation)：不导致请求进程阻塞
    同步I/O操作：
        阻塞式I/O模型、非阻塞式I/O模型、信号驱动式I/O模型
    异步I/O操作：
        异步I/O模型
24. select
    select参数可能设定：
    1）永远等待
    2）等待一端固定时间
    3）根本不等待
    4）所有需要关心的文件描述符重新加入到描述符集中
    5）其上有一个套接字错误待处理时，读/写操作将不阻塞，立即返回-1
    6）FD_SETSIZE的值需要重新编译内核才能起作用
25. shutdown
    写入一个路由套接字中的所有数据都被称作为同一个主机上所有路由套接字的可能输入环回。
    避免方法如下：
    1）指定shutdown函数的参数为SHUT_RD
    2）关闭SO_USELOOPBACK套接字选项
26. poll
    poll识别三类数据：普通(normal)，优先级带(priority band)和高优先级(high priority)。
    1）所有正规TCP数据和所有UDP数据都被认为是普通数据
    2）TCP带外数据被认为是优先级带数据
    3）TCP连接的读半部关闭时，也被认为是普通数据，随后的读操作将返回0
    4）TCP连接存在错误既可认为是普通数据，也可认为是错误(POLLERR)，
        无论哪种情况，随后的读操作将返回-1，并把errno置成合适的值
    5）在监听套接字上有新的连接可用，既可认为是普通数据，也可认为是优先级数据，
        大多数现实视为普通数据
    6）非阻塞式connect的完成被认为是使相应套接字可写
    7）检查POLLERR的原因在于有些实现在一个连接上接收到RST时返回的是POLLERR事件，
        而其他实现返回的只是POLLRDNORM事件
27. getsockopt/setsockopt
    套接字选项粗分为两大基本类型：
    1) 启用或禁止某个特性的二元选项（标志选项）
    2) 取得并返回我们可以设置或检查的特定值的选项（值选项）
        标有“标志”的列指出一个选项是否为标志选项，当给这些标志选项调用getsockopt函数时，
        *optval是一个整数。*optval中返回的值为0表示相应选项被禁止，不为0表示相应选项被启用，
        setsockopt类似。如果标识列没有标志选项，表示用户进程与系统之间传递所指定数据类型的值
    选项名称　　　　　　说明　　　　　　　　　　　　数据类型                标志
    =============================================================================
    　　　　　　　　　　SOL_SOCKET
    -----------------------------------------------------------------------------
    SO_BROADCAST　　　　允许发送广播数据　　　　　　int                     *
    SO_DEBUG　　　　　　允许调试　　　　　　　　　　int                     *
    SO_DONTROUTE　　　　不查找路由　　　　　　　　　int                     *
    SO_ERROR　　　　　　获得套接字错误　　　　　　　int
    SO_KEEPALIVE　　　　保持连接　　　　　　　　　　int                     *
    SO_LINGER　　　　　 延迟关闭连接　　　　　　　　struct linger
    SO_OOBINLINE　　　　带外数据放入正常数据流　　　int                     *
    SO_RCVBUF　　　　　 接收缓冲区大小　　　　　　　int
    SO_SNDBUF　　　　　 发送缓冲区大小　　　　　　　int
    SO_RCVLOWAT　　　　 接收缓冲区下限　　　　　　　int
    SO_SNDLOWAT　　　　 发送缓冲区下限　　　　　　　int
    SO_RCVTIMEO　　　　 接收超时　　　　　　　　　　struct timeval
    SO_SNDTIMEO　　　　 发送超时　　　　　　　　　　struct timeval
    SO_REUSERADDR　　　 允许重用本地地址      　　　int                     *
    SO_REUSERPORT　　　 允许重用本地端口      　　　int                     *
    SO_TYPE　　　　　　 获得套接字类型　　　　　　　int
    SO_USELOOPBACK　　　路由套接字取得数据副本　　　int                     *
    =============================================================================
    　　　　　　　　　　IPPROTO_IP
    -----------------------------------------------------------------------------
    IP_HDRINCL　　　　　在数据包中包含IP首部　　　　int                     * 
    IP_OPTINOS　　　　　IP首部选项　　　　　　　　　int
    IP_TOS　　　　　　　服务类型 
    IP_TTL　　　　　　　生存时间　　　　　　　　　　int
    =============================================================================
    　　　　　　　　　　IPPRO_TCP
    -----------------------------------------------------------------------------
    TCP_MAXSEG　　　　　TCP最大数据段的大小　　　　 int
    TCP_NODELAY　　　　 不使用Nagle算法　　　　　　 int                     *
    =============================================================================

    返回说明：
    成功执行时，返回0。失败返回-1，errno被设为以下的某个值：
    1）EBADF：sock不是有效的文件描述词
    2）EFAULT：optval指向的内存并非有效的进程空间
    3）EINVAL：在调用setsockopt()时，optlen无效
    4）ENOPROTOOPT：指定的协议层不能识别选项
    5）ENOTSOCK：sock描述的不是套接字

    SO_RCVBUF和SO_SNDBUF每个套接口都有一个发送缓冲区和一个接收缓冲区，
    使用这两个套接口选项可以改变缺省缓冲区大小。
    注：
    当设置TCP套接口接收缓冲区的大小时，函数调用顺序是很重要的，因为TCP的窗口规模选项是在
    建立连接时用SYN与对方互换得到的。对于客户，SO_RCVBUF选项必须在connect之前设置；
    对于服务器，SO_RCVBUF选项必须在listen前设置。

    套接字状态：
    如果想在三路握手完成时确证某些套接字是给已连接套接字设置的，必须先给监听套接字设置

    a. SO_ BROADCAST:
    开启或禁止进程发送广播消息。只有数据报套接字支持广播，并且还必须是支持广播消息的网络上
    （以太网、令牌环网），无法在点对点链路上进行广播，也不能在基于连接的传输协议（TCP、SCTP）
    之上进行广播。可以有效防止进程在其对应程序没有设计成可广播时就发送广播数据报

    b. SO_DEBUG
    仅TCP支持，内核将为TCP在该套接字发送和接收的所有的分组保留详细跟踪信息，这些信息保存在
    内核的某个环形缓冲区中，并可使用trpt程序进行检查

    c. SO_DONTROUTE
    外出的分组绕过底层协议的正常路由机制。路由守护进程（routed和gated）经常使用此选项。

    d. SO_ERROR
    当一个套接字上发生错误时，so_error变量被设置为标准Unix Exxx值中的一个，内核通过两种方式
    立即通知进程错误：
    1）进程阻塞在对该套接字的select调用上，select立即返回-1
    2）进程使用信号驱动式I/O模型，产生SIGIO信号

    e. SO_KEEPALIVE
    此选项被设置后，如果2小时内在该套接字的任意方向上都没有数据交换，TCP就自动给对端发送一个
    保持存活探测分节(keep-alive probe)，对端必须响应，会导致三种情况：
    1）对端以期望的ACK响应
    2）对端以RST响应，它告知本端TCP：对端已崩溃且已重新启动，待处理错误被置为ECONNRESET
    3）对端保持存活探测分节没有任何响应。另外发送8个探测分节，两两相隔75秒，TCP在发出第一个
        探测分节后11分15秒内若没有得到任何响应则放弃
    如果没有收到TCP探测分节的响应，套接字的待处理错误就被置为ETIMEOUT，如果收到一个ICMP错误
    作为某个探测分节的响应，待处理错误就被置为EHOSTUNREACH。
    如果把无活动周期从2小时改为（比如）15分钟，将会影响到该主机上所有开启了本选项的套接字
    某个中间路由崩溃15分钟是可能的，这段时间正好与主机的11分15秒的保持存活探测周期完全重叠

    f. SO_LINGER
    本选项指定close函数对面向连接的协议（SCTP和TCP）如何操作。
    1）l_onoff为0：关闭本选项，l_linger的值被忽略，close立即返回，但是如果有数据残留在套接字
        发送缓冲区中，系统试着把这些数据发送给对端
    2）l_onoff的值非0，且l_linger的值为0：那么close某个连接时，TCP将中止该连接。但是存在
        在2MSL秒内创建该连接的另一个化身，导致来自刚被终止的连接上的旧的重复分节被不正确
        地递送到新的化身上
    3）l_onoff的值非0，且l_linger的值非0：那么当套接字关闭时，内核将拖延一段时间，直至发
        送缓冲区中的数据全部发送完毕且被对方确认或延滞时间到。如果套接字被设置为非阻塞型，
        将不等待close完成
    以上3）情况发生时，从close的成功返回只能得到对端TCP确认，不能得知对端应用进程是否已读取数据

    g. SO_OOBINLINE
    选项开启时，带外数据被留在正常的输入队列中，接收函数的MSG_OOB标志不能用来读带外数据

    h. SO_RECVBUF和SO_SNDBUF
    这两个套接字选项允许我们改变这两个缓冲区的默认大小。
    如果主机支持NFS，UDP发送缓冲区的大小默认为9000字节左右，UDP接收缓冲区的默认大小为40,000字节
    TCP套接字缓冲区的大小至少应该是相应连接的MSS值的四倍，典型的缓冲区大小默认值是8192字节或更大，
    典型的MSS值为512或1460
    管道的容量称为带宽-延迟积（bandwidth-delay product）：带宽（bytes/s）和RTT（秒）相乘

    i. SO_RECVLOWAT和SO_SNDLOWAT
    每个套接字还有一个接收低水位标记和一个发送低水位标记。
    TCP/SCTP默认的接收低水位标记值为1，TCP套接字默认的发送缓冲区空间值为2048。
    UDP套接字的发送缓冲区大小大于该套接字的低水位标记，就可写，且没有发送缓冲区

    j. SO_RCVTIMEO和SO_SNDTIMEO
    这两个选项允许给套接字的接收和发送设置一个超时值。默认情况下这两个超时都是禁止的

    k. SO_REUSEADDR和SO_REUSEPORT
    1）SO_REUSEADDR允许端口重复使用，即允许服务器重启
    2）SO_REUSEADDR允许同一端口上启动同一服务器的多个实例，每个实例绑定不同的IP地址
        有些操作系统不允许对已经绑定了通配地址的端口在捆绑任何“更为明确”的地址
    3）SO_REUSEADDR允许单个进程捆绑同一端口到多个套接字上，每次绑定指定不同的IP地址
    4）SO_REUSEADDR允许绑定到某个套接字上的IP地址和端口绑定到另一个套接字上
    5）SO_REUSEPORT允许完全重复的捆绑
    6）被捆绑的IP地址是一个多播地址，SO_REUSEADDR和SO_REUSEPORT等效

    l. SO_TYPE
    返回套接字类型，整数值是SOCK_STREAM或SOCK_DGRAM的值

    m. SO_USELOOPBACK
    仅用于路由域（AF_ROUTE）的套接字，默认设置为打开（唯一一个默认值为打开而不是关闭
    的SO_xxx二元套接字选项），相应套接字将接收其上发送的任何数据报的一个副本

    n. TCP_MAXSEG
    本选项允许获取或设置TCP连接的最大分节大小（MSS），返回值是我们的TCP可以发送给对端的最大数量

    o. TCP_NODELAY
    本选项开启将禁止TCP的Nagle算法，默认情况下该算法是启动的
    两种不适合Nagle算法和ACK延滞算法的情况：
    1）服务器不在相反方向产生数据
    2）客户以若干小片数据向服务器发送单个逻辑请求
    三种修正方法：
    1）使用writev而不是两次调用write，此方法首选
    2）合并发送请求，只调用一次write
    3）设置TCP_NODELAY，多次调用write，此方法最不可取
