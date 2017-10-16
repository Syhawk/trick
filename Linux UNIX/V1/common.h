#ifndef COMMON_H
#define COMMON_H

#include <list>
#include <algorithm>

// C2S
#define C2S_LOGIN           0x01
#define C2S_LOGOUT          0x02
#define C2S_ONLINE_USER     0x03

// CLIENT
#define CLIENT_ONLINE       0x01
#define CLIENT_OFFLINE      0x02

// MSG
#define MSG_LEN             1024
#define NAME_LEN            32

// S2C
#define S2C_LOGIN_OK        0x01
#define S2C_ALREADY_LOGINED 0x02
#define S2C_SOMEONE_LOGIN   0x03
#define S2C_SOMEONE_LOGOUT  0x04
#define S2C_ONLINE_USER     0x05
#define S2C_LOGIN_FAIL      0x05

// C2C
#define C2C_CHAT            0x06

typedef struct Message {
    int cmd;
    char body[MSG_LEN];
}MESSAGE;

typedef struct UserInfo {
    unsigned int userstate;
    char username [NAME_LEN];
    unsigned int ip;
    unsigned short port;
}USER_INFO;

typedef struct ChatMsg {
    char username[NAME_LEN];
    char msg[MSG_LEN];
}CHAT_MSG;

typedef std::list<USER_INFO> UserList;

#endif  // COMMON_H
