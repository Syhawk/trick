#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netdb.h>

using namespace std;

int getlocalip(char* ip) {
    // Get hostname.
    char host[100];
    if (gethostname(host, sizeof(host)) < 0) {
        printf("get host name failed.\n");
        return -1;
    }
    printf("hostname is: %s\n", host);

    struct hostent* hep = NULL;
    if ((hep = gethostbyname(host)) == NULL) {
        printf("get host by name failed.\n");
        return -1;
    }

    // Get all ip address.
    int i = 0;
    while (hep->h_addr_list[i] != NULL) {
        printf("%s\n", inet_ntoa(*((struct in_addr*)(hep->h_addr_list[i]))));
        ++i;
    }
    
    // Get ip address.
    strcpy(ip, inet_ntoa(*((struct in_addr*)hep->h_addr)));
    return 0;
}

int main() {
    char ip[16];
    int err = getlocalip(ip);
    if (err < 0) {
        printf("get lcoal ip failed.\n");
        return 0;
    }
    
    printf("local ip is: %s\n", ip);
    
    return 0;
}
