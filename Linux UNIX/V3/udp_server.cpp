/*
*   1. int msgget(key_t key, int msgflg);
*   2. int msgctl(int msqid, int cmd, struct msqid_ds *buf);
*
*   ipcs
*   ipcrm
*   cat /proc/sys/kernel/msgmax msgmni msgmnb
*/

#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "interface.h"
#include "common.h"


int main() {
    int msgid;
    msgid = msgget(123, 0600 | IPC_CREAT);
    // msgid = msgget(IPC_PRIVATE, 0666 | IPC_CREAT | IPC_EXCL);
    // msgid = msgget(1234, 0);
    if (msgid == -1) {
        ERR_EXIT("msgget error");
    }
    printf("msgid = %d\n", msgid);
    struct msqid_ds buf;
    if (msgctl(msgid, IPC_STAT, &buf) < 0) {
        ERR_EXIT("msgctl stat");
    }
    printf("mode = %o\n", buf.msg_perm.mode);
    printf("cur num bytes in queue = %lu\n", buf.__msg_cbytes);
    printf("cur num message in queue = %lu\n", buf.msg_qnum);
    printf("max num of bytes = %lu\n", buf.msg_qbytes);

    buf.msg_perm.mode = 0466;
    if (msgctl(msgid, IPC_SET, &buf) < 0) {
        ERR_EXIT("msgctl set");
    }
    printf("mode = %o\n", buf.msg_perm.mode);

    if (msgctl(msgid, IPC_RMID, NULL) < 0) {
        ERR_EXIT("msgctl rm");
    }

    return 0;
}
