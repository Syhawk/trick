#include <string.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>

#include "interface.h"
#include "common.h"

UserList client_list;

void chat_srv(int sock);
void do_login(int sock, struct Message& message, struct sockaddr_in* cli_addr);
void do_logout(int sock, struct Message& message, struct sockaddr_in* cli_addr);
void do_getlist(int sock, struct sockaddr_in* cli_addr);
void sendlist(int sock, struct sockaddr_in* cli_addr);
void sendnotice(int sock, struct Message& msg);

void chat_serv(int sock) {
    struct sockaddr_in cli_addr;
    socklen_t client_len;
    struct Message msg;

    while (1) {
        memset(&msg, 0, sizeof(msg));
        client_len = sizeof(cli_addr);

        int err = recvfrom(sock, &msg, sizeof(struct Message), 0, (struct sockaddr*)(&cli_addr), &client_len);
        errno_intr(err, "recvfrom error");
        if (err < 0) {
            continue;
        }

        int cmd = ntohl(msg.cmd);
        switch(cmd) {
            case C2S_LOGIN:
                do_login(sock, msg, &cli_addr);
                break;
            case C2S_LOGOUT:
                do_logout(sock, msg, &cli_addr);
                break;
            case C2S_ONLINE_USER:
                do_getlist(sock, &cli_addr);
                break;
            default:
                break;
        }
    }
    close(sock);
}

void do_login(int sock, struct Message& msg, struct sockaddr_in* cli_addr) {
    UserInfo user;
    user.userstate = CLIENT_ONLINE;
    strcpy(user.username, msg.body);
    user.ip = cli_addr->sin_addr.s_addr;
    user.port = cli_addr->sin_port;

    // Search user.
    UserList::iterator itr = client_list.begin();
    for (; itr != client_list.end(); ++itr) {
        if (strcmp(itr->username, user.username) == 0) {
            break;
        }
    }

    struct Message reply_msg;
    memset(&reply_msg, 0, sizeof(reply_msg));
    // Can't find user.
    if (itr == client_list.end() || itr->userstate == CLIENT_OFFLINE) {
        // Login success.
        printf("has a user login: [%s <-> %s:%d]\n", user.username, inet_ntoa(cli_addr->sin_addr), ntohs(cli_addr->sin_port));
        reply_msg.cmd = htonl(S2C_LOGIN_OK);
        int err = sendto(sock, &reply_msg, sizeof(reply_msg), 0, (struct sockaddr*)cli_addr, sizeof(struct sockaddr));        
        errno_intr(err, "sendto login success info error");
    } else {    // Find user.
        reply_msg.cmd = htonl(S2C_ALREADY_LOGINED);
        if (itr->ip != user.ip || itr->port != user.port) {
            // User login fail.
            reply_msg.cmd = htonl(S2C_LOGIN_FAIL);
            int err = sendto(sock, &reply_msg, sizeof(reply_msg), 0, (struct sockaddr*)cli_addr, sizeof(struct sockaddr));
            errno_intr(err, "sendto user about login info error.");
            return;
        }

        // Old user login success.
        printf("user [%s] has already logined.\n", user.username);
        int err = sendto(sock, &reply_msg, sizeof(reply_msg), 0, (struct sockaddr*)cli_addr, sizeof(struct sockaddr));
        errno_intr(err, "sendto notice about login success info error");
    }

    // Old user relogin or new user login.
    if (itr != client_list.end()) {
        *itr = user;
    } else {
        client_list.push_back(user);
    }

    // Send login notice.
    memset(&reply_msg, 0, sizeof(reply_msg));
    reply_msg.cmd = htonl(S2C_SOMEONE_LOGIN);
    memcpy(reply_msg.body, &user, sizeof(struct UserInfo));
    sendnotice(sock, reply_msg);

    // Send online user list.
    sendlist(sock, cli_addr);
}

void do_logout(int sock, struct Message& msg, struct sockaddr_in* cli_addr) {

    UserList::iterator itr = client_list.begin();
    for (; itr != client_list.end(); ++itr) {
        if (strcmp(msg.body, itr->username) == 0) {
            itr->userstate = CLIENT_OFFLINE;
            break;
        }
    }

    printf("has a user logout: [%s <-> %s:%d]\n", msg.body, inet_ntoa(cli_addr->sin_addr), ntohs(cli_addr->sin_port));
    // Send logout notice.
    msg.cmd = htonl(S2C_SOMEONE_LOGOUT);
    memcpy(msg.body, &(*itr), sizeof(struct UserInfo));
    sendnotice(sock, msg);
}

void do_getlist(int sock, struct sockaddr_in* cli_addr) {
    struct Message reply_msg;
    memset(&reply_msg, 0, sizeof(reply_msg));
    reply_msg.cmd = htonl(S2C_ONLINE_USER);
    int err = sendto(sock, &reply_msg, sizeof(reply_msg), 0, (struct sockaddr*)cli_addr, sizeof(struct sockaddr));
    errno_intr(err, "sendto user about getlist label info error.");

    // Send online user list.
    sendlist(sock, cli_addr);
}

void sendlist(int sock, struct sockaddr_in* cli_addr) {
    struct Message reply_msg;
    memset(&reply_msg, 0, sizeof(reply_msg));
    reply_msg.cmd = htonl(S2C_ONLINE_USER);

    // Send online user num.
    int count = 0;
    for (UserList::iterator itr = client_list.begin(); itr != client_list.end(); ++itr) {
        if (itr->userstate == CLIENT_ONLINE) {
            ++count;
        }
    }

    count = htonl(count);
    int err = sendto(sock, &count, sizeof(count), 0, (struct sockaddr*)cli_addr, sizeof(struct sockaddr));
    errno_intr(err, "sendto user about user list num info error.");

    // Send online user info.
    for (UserList::iterator itr = client_list.begin(); itr != client_list.end(); ++itr) {
        if (itr->userstate == CLIENT_OFFLINE) {
            continue;
        }

        err = sendto(sock, &(*itr), sizeof(UserInfo), 0, (struct sockaddr*)cli_addr, sizeof(struct sockaddr));
        errno_intr(err, "sendto online user info error");
    }
}

void sendnotice(int sock, struct Message& msg) {
    // Send notice to user.
    struct UserInfo* ptr = (struct UserInfo*)msg.body;
    for (UserList::iterator itr = client_list.begin(); itr != client_list.end(); ++itr) {
        if (itr->userstate == CLIENT_OFFLINE || strcmp(itr->username, ptr->username) == 0) {
            continue;
        }

        struct sockaddr_in peer_addr;
        memset(&peer_addr, 0, sizeof(peer_addr));
        peer_addr.sin_family = AF_INET;
        peer_addr.sin_addr.s_addr = itr->ip;
        peer_addr.sin_port = itr->port;

        int err = sendto(sock, &msg, sizeof(msg), 0, (struct sockaddr*)(&peer_addr), sizeof(struct sockaddr));
        errno_intr(err, "sendto user notice error");
    }
}

int main() {
    // Create socket.
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        ERR_EXIT("server create socket error");
    }

    // Set ip address and port.
    struct sockaddr_in server_ip;
    server_ip.sin_family = AF_INET;
    server_ip.sin_port = htons(5555);
    server_ip.sin_addr.s_addr = htonl(INADDR_ANY);
    memset(server_ip.sin_zero, 0, sizeof(server_ip.sin_zero));

    // Bind ip address and port to socket.
    int err = bind(sock, (struct sockaddr*)(&server_ip), sizeof(struct sockaddr));
    if (err == -1) {
        close(sock);
        ERR_EXIT("server bind error");
    }

    chat_serv(sock);

    return 0;
}
