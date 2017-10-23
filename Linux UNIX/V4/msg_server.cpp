/*
*   1. int msgget(key_t key, int msgflg);
*   2. int msgctl(int msqid, int cmd, struct msqid_ds *buf);
*   3. int msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg);
*   4. ssize_t msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg);
*
*   ipcs
*   ipcrm
*   cat /proc/sys/kernel/msgmax msgmni msgmnb
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

#include "interface.h"
#include "common.h"

#define MSGMAX 8192

struct MsgBuf {
    long mtype;
    char mtext[MSGMAX];
};

void echo_srv(int msgid) {
    while (1) {
        struct MsgBuf msg;
        memset(&msg, 0, sizeof(msg));
        int size = msgrcv(msgid, &msg, MSGMAX, 1, 0);
        if (size < 0) {
            ERR_EXIT("msg recv error");
        }

        int pid;
        pid = *((int*)msg.mtext);
        msg.mtype = pid;
        
        printf("data is: %s", msg.mtext + 4);

        size = msgsnd(msgid, &msg, 4 + strlen(msg.mtext + 4), 0);
        if (size < 0) {
            ERR_EXIT("msg snd error");
        }
    }
}

int main(int argc, char* argv[]) {
    int msgid;
    msgid = msgget(1234, IPC_CREAT | 0666);
    if (msgid < 0) {
        ERR_EXIT("msg create error");
    }

    echo_srv(msgid);

    return 0;
}
