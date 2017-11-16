#include "unp.h"
#include "wrapsock.h"
#include "error.h"


int main(int argc, char** argv) {
    int sockfd;
    sockfd = Socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);

    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    Bind(sockfd, (SA*)&servaddr, sizeof(servaddr));

    struct sctp_event_subscribe events;
    bzero(&events, sizeof(events));
    events.sctp_data_io_event = 1;
    Setsockopt(sockfd, IPPROTO_SCTP, SCTP_EVENTS, &events, sizeof(events));

    Listen(sockfd, LISTENQ);

    char buf[MAXLINE] = {0};
    struct sockaddr_in cliaddr;
    socklen_t len;
    struct sctp_sndrcvinfo sri;
    int msg_flags;
    int stream_increment = 1;

    while (1) {
        len = sizeof(struct sockaddr_in);
        int fd = accept(sockfd, (SA*)&cliaddr, &len);
        //int fd = sockfd;
        printf("sockfd = %d\n", fd);
        int nread = Sctp_recvmsg(fd, buf, sizeof(buf),
                (SA*)&cliaddr, &len, &sri, &msg_flags);

        if (stream_increment) {
            sri.sinfo_stream += 1;
            /*
            if (sri.sinfo_stream >=
                    Sctp_get_no_strms(sockfd, (SA*)&cliaddr, len)) {
                sri.sinfo_stream = 0;
            }
            */
        }
    
        printf("size is %d, buf is %s", nread, buf);

        Sctp_sendmsg(fd, buf, nread, (SA*)&cliaddr, len,
                sri.sinfo_ppid, sri.sinfo_flags, sri.sinfo_stream, 0, 0);

        bzero(buf, sizeof(buf));
        break;
    }

    Close(sockfd);
    
    return 0;
}
