#include <string.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <stdlib.h>

#include "interface.h"
#include "common.h"

// User list.
UserList client_list;
// User name.
char username[NAME_LEN];

void chat_cli(int sock);
void do_someone_login(struct Message& msg);
void do_someone_logout(struct Message& msg);
void do_getlist(int sock);
void do_chat(struct Message& msg);

void parse_cmd(int sock, struct sockaddr_in* serv_addr, char* cmdline);
int sendmsgto(int sock, char* peer_name, char* msg);
void show_command();

void show_command() {
    printf("\nCommands are:\n");
    printf("send username msg\n");
    printf("list\n");
    printf("exit\n");
    printf("\n");
}

void parse_cmd(int sock, struct sockaddr_in* serv_addr, char* cmdline) {
    char cmd[10] = {0};
    char* p = strchr(cmdline, ' ');
    if (p != NULL) {
        *p = '\0';
    }

    strcpy(cmd, cmdline);
    if (strcmp(cmd, "exit") == 0) {
        struct Message msg;
        memset(&msg, 0, sizeof(msg));
        msg.cmd = htonl(C2S_LOGOUT);
        strcpy(msg.body, username);

        int err = sendto(sock, &msg, sizeof(msg), 0, (struct sockaddr*)serv_addr, sizeof(struct sockaddr));
        errno_intr(err, "send logout request fail");
        printf("user [%s] has logout server\n", username);
        exit(EXIT_SUCCESS);
    }
    if (strcmp(cmd, "send") == 0) {
        char peer_name[NAME_LEN] = {0};
        char msg[MSG_LEN] = {0};
        
        while (*(++p) == ' ');
        char *p2 = strchr(p, ' ');
        if (p2 == NULL) {
            printf("bad command\n");
            show_command();
        }
        *p2 = '\0';
        strcpy(peer_name, p);
        
        while (*(++p2) == ' ');
        strcpy(msg, p2);
        sendmsgto(sock, peer_name, msg);
        return;
    }
    if (strcmp(cmd, "list") == 0) {
        struct Message msg;
        memset(&msg, 0, sizeof(msg));
        msg.cmd = htonl(C2S_ONLINE_USER);
        int err = sendto(sock, &msg, sizeof(msg), 0, (struct sockaddr*)serv_addr, sizeof(struct sockaddr));
        errno_intr(err, "send get user list request fail");
        return;
    }

    printf("bad command\n");
    show_command();
}

int sendmsgto(int sock, char* peer_name, char* msg) {
    if (strcmp(peer_name, username) == 0) {
        printf("can't send message to self.\n");
        return -1;
    }

    UserList::iterator itr = client_list.begin();
    for (; itr != client_list.end(); ++itr) {
        if (strcmp(itr->username, peer_name) == 0) {
            break;
        }
    }
    
    if (itr == client_list.end() || itr->userstate == CLIENT_OFFLINE) {
        printf("user [%s] has not logined server.\n", peer_name);
        return -1;
    }

    struct ChatMsg cht_msg;
    memset(&cht_msg, 0, sizeof(cht_msg));
    strcpy(cht_msg.username, username);
    strcpy(cht_msg.msg, msg);

    struct Message message;
    memset(&message, 0, sizeof(message));
    message.cmd = htonl(C2C_CHAT);
    memcpy(message.body, &cht_msg, sizeof(cht_msg));

    struct sockaddr_in peer_addr;
    memset(&peer_addr, 0, sizeof(peer_addr));
    peer_addr.sin_family = AF_INET;
    peer_addr.sin_addr.s_addr = itr->ip;
    peer_addr.sin_port = itr->port;

    int err = sendto(sock, &message, sizeof(message), 0, (struct sockaddr*)(&peer_addr), sizeof(struct sockaddr));
    errno_intr(err, "send message to peeraddr error");
    printf("send message [%s] to user [%s].\n", msg, peer_name);
    return 0;
}

void do_someone_login(struct Message& msg) {
    struct UserInfo* user = (struct UserInfo*)msg.body;

    printf("user [%s] has logined server.\n", user->username);
    user->userstate = CLIENT_ONLINE;
    client_list.push_back(*user);
}

void do_someone_logout(struct Message& msg) {
    UserInfo* ptr = (struct UserInfo*)msg.body;
    UserList::iterator itr = client_list.begin();
    for (; itr != client_list.end(); ++itr) {
        if (strcmp(itr->username, ptr->username) == 0) {
            itr->userstate = CLIENT_OFFLINE;
            break;
        }
    }

    printf("user [%s] has logout server.\n", itr->username);
}

void do_chat(struct Message& msg) {
    ChatMsg* cht_msg = (ChatMsg*)msg.body;
    printf("recv a msg [%s] from [%s].\n", cht_msg->msg, cht_msg->username);
    // recvform(sock, &count, sizeof(int), 0, NULL, NULL);
}

void do_getlist(int sock) {
    int count;
    int err = recvfrom(sock, &count, sizeof(count), 0, NULL, NULL);
    errno_intr(err, "recv user list num from server fail");

    client_list.clear();
    count = ntohl(count);
    for (int i = 0; i < count; ++i) {
        struct UserInfo user;
        err = recvfrom(sock, &user, sizeof(user), 0, NULL, NULL);
        errno_intr(err, "recv user info from server fail");

        user.userstate = CLIENT_ONLINE;
        client_list.push_back(user);
        in_addr addr;
        addr.s_addr = user.ip;
        printf("client %d: [%s <-> %s:%d]\n", i + 1, user.username, inet_ntoa(addr), ntohs(user.port));
    }
}

void chat_cli(int sock) {
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(5555);

    struct Message msg;
    while (1) {
        memset(username, 0, sizeof(username));
        printf("please input your name:");
        fflush(stdout);
        scanf("%s", username);
        
        memset(&msg, 0, sizeof(msg));
        msg.cmd = htonl(C2S_LOGIN);
        strcpy(msg.body, username);

        // Sendto server about user login request.
        int err = sendto(sock, &msg, sizeof(msg), 0, (struct sockaddr*)(&serv_addr), sizeof(struct sockaddr));
        errno_intr(err, "sendto server about user login request fail");

        // Recv response from server about user login request.
        memset(&msg, 0, sizeof(msg));
        err = recvfrom(sock, &msg, sizeof(msg), 0, NULL, NULL);
        errno_intr(err, "recv response from server about user login request fail");

        int cmd = ntohl(msg.cmd);
        if (cmd == S2C_LOGIN_OK || cmd == S2C_ALREADY_LOGINED) {
            printf("user [%s] login in success.\n", username);
            break;
        }
        printf("user [%s] login in failed, please use another username.\n", username);
    }

    // Recv online user num.
    int count = 0;
    int err = recvfrom(sock, &count, sizeof(count), 0, NULL, NULL);
    errno_intr(err, "recv online user num from server fail");
    count = ntohl(count);
    printf("has %d users logined server\n", count);
    // Recv online user info.
    for (int i = 0; i < count; ++i) {
        UserInfo user;
        err = recvfrom(sock, &user, sizeof(UserInfo), 0, NULL, NULL);
        errno_intr(err, "recv user info from server failed");

        user.userstate = CLIENT_ONLINE;
        client_list.push_back(user);
        in_addr addr;
        addr.s_addr = user.ip;
        printf("user %d: [%s <-> %s:%d]\n", i + 1, user.username, inet_ntoa(addr), ntohs(user.port));
    }
    
    show_command();
    
    fd_set rset;
    FD_ZERO(&rset);
    int fd = fileno(stdin);
    while (1) {
        FD_SET(fd, &rset);
        FD_SET(sock, &rset);
        
        int nready = select(sock + 1, &rset, NULL, NULL, NULL);
        if (nready == -1) {
            ERR_EXIT("select file descriptor error");
        }
        if (nready == 0) {
            continue;
        }

        if (FD_ISSET(sock, &rset)) {
            err = recvfrom(sock, &msg, sizeof(msg), 0, NULL, NULL);
            int cmd = ntohl(msg.cmd);
            switch(cmd) {
                case S2C_SOMEONE_LOGIN:
                    do_someone_login(msg);
                    break;
                case S2C_SOMEONE_LOGOUT:
                    do_someone_logout(msg);
                    break;
                case S2C_ONLINE_USER:
                    do_getlist(sock);
                    break;
                case C2C_CHAT:
                    do_chat(msg);
                    break;
                default:
                    break;
            }
            
        } else if (FD_ISSET(fd, &rset)) {
            char cmdline[MSG_LEN];
            memset(cmdline, 0, sizeof(cmdline));
            if (fgets(cmdline, sizeof(cmdline), stdin) == NULL) {
                continue;
            }
            if (cmdline[0] == '\n') {
                continue;
            }
            cmdline[strlen(cmdline) - 1] = '\0';
            parse_cmd(sock, &serv_addr, cmdline);
        }
    }

    close(sock);
}


int main() {
    // Create socket.
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        ERR_EXIT("create socket error");
    }

    chat_cli(sock);

    return 0;
}
