#include "wrapsock.h"
#include "error.h"

int main(int argc, char** argv) {
    int echo_to_all = 0;
    if (argc >= 2) {
        echo_to_all = atoi(argv[1]);
    }

	int sockfd;
	sockfd = Socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);

	struct sockaddr_in servaddr;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

    struct sctp_event_subscribe events;
    bzero(&events, sizeof(events));
    events.sctp_data_io_event = 1;
    Setsockopt(sockfd, IPPROTO_SCTP, SCTP_EVENTS, &events, sizeof(events));

    if (echo_to_all == 0) {
        Sctp_cli_echo(sockfd, (SA*)&servaddr, sizeof(servaddr));
    } else {
        Sctp_cli_echoall(sockfd, (SA*)&servaddr, sizeof(servaddr));
    }

	Close(sockfd);

	return 0;
}
