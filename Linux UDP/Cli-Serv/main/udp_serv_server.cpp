#include "wrapsock.h"
#include "error.h"


int main(int argc, char** argv) {
	int sockfd;
    if (argc == 2) {
        sockfd = Udp_server(NULL, argv[1], NULL);
    } else if (argc == 3) {
        sockfd = Udp_server(argv[1], argv[2], NULL);
    } else {
        err_quit("usage: udp_serv_server [<host>] <service/port>");
    }

    udp_serv_echo(sockfd);

    Close(sockfd);

    return 0;
}

