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

void echo_cli(int msgid) {
    struct MsgBuf msg;
    memset(&msg, 0, sizeof(msg));
    int pid = getpid();
    *((int*)msg.mtext) = pid;
    msg.mtype = 1;

    while (fgets(msg.mtext + 4, (MSGMAX - 4), stdin) != NULL) {
        int size = msgsnd(msgid, &msg, 4 + strlen(msg.mtext + 4), 0);
        if (size < 0) {
            ERR_EXIT("msgsnd error");
        }

        memset(&msg, 0, sizeof(msg));
        size = msgrcv(msgid, &msg, MSGMAX, pid, 0);
        if (size < 0) {
            ERR_EXIT("msg recv error");
        }
        printf("%s", msg.mtext + 4);

        memset(msg.mtext + 4, 0, sizeof(msg.mtext + 4));
        msg.mtype = 1;
    }
}

int main(int argc, char* argv[]) {
    int msgid;
    msgid = msgget(1234, 0);
    if (msgid < 0) {
        ERR_EXIT("msgid get error");
    }
    
    echo_cli(msgid);

    return 0;
}
