/*
*   1. int msgget(key_t key, int msgflg);
*   2. int msgctl(int msqid, int cmd, struct msqid_ds *buf);
*   3. int msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg);
*   4. ssize_t msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg);
*   5. int getopt(int argc, char * const argv[], const char *optstring);
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


int main(int argc, char* argv[]) {
    int id = 0;
    int msgid = -1;
    int len = 0;
    int flg = 0;
    int type = 0;
    int opt;
    struct msqid_ds buf;
    while ((opt = getopt(argc, argv, "c:g:d:m:s:l:r:t:")) != -1) {
        switch(opt) {
            case 'c':
                id = atoi(optarg);
                msgid = msgget(id, 0600 | IPC_CREAT); // | IPC_EXCL
                if (msgid < 0) {
                    ERR_EXIT("create error");
                }
                break;
            case 'g':
                id = atoi(optarg);
                msgid = msgget(id, 0);
                if (msgid < 0) {
                    ERR_EXIT("get msgid error");
                }

                if (msgctl(msgid, IPC_STAT, &buf) < 0) {
                    ERR_EXIT("msgctl stat error");
                }
                printf("mode = %o\n", buf.msg_perm.mode);
                printf("cur num bytes in queue = %lu\n", buf.__msg_cbytes);
                printf("cur num message in queue = %lu\n", buf.msg_qnum);
                printf("max num of bytes = %lu\n", buf.msg_qbytes);
                break;
            case 'd':
                id = atoi(optarg);
                msgid = msgget(id, 0);
                if (msgid < 0) {
                    ERR_EXIT("get msgid error");
                }
                if (msgctl(msgid, IPC_RMID, NULL) < 0) {
                    ERR_EXIT("msgctl rm error");
                }
                break;
            case 'm':
                id = atoi(optarg);
                msgid = msgget(id, 0);
                if (msgid < 0) {
                    ERR_EXIT("get msgid error");
                }

                if (msgctl(msgid, IPC_STAT, &buf) < 0) {
                    ERR_EXIT("msgctl stat error");
                }

                buf.msg_perm.mode = 0666;
                if (msgctl(msgid, IPC_SET, &buf) < 0) {
                    ERR_EXIT("msgctl set error");
                }

                if (msgctl(msgid, IPC_STAT, &buf) < 0) {
                    ERR_EXIT("msgctl stat error");
                }
                printf("mode = %o\n", buf.msg_perm.mode);
                printf("cur num bytes in queue = %lu\n", buf.__msg_cbytes);
                printf("cur num message in queue = %lu\n", buf.msg_qnum);
                printf("max num of bytes = %lu\n", buf.msg_qbytes);
                break;
            case 's':
                flg = 1;
                id = atoi(optarg);
                msgid = msgget(id, 0);
                if (msgid < 0) {
                    ERR_EXIT("get msgid error");
                }
                break;
            case 'r':
                flg = 2;
                id = atoi(optarg);
                msgid = msgget(id, 0);
                if (msgid < 0) {
                    ERR_EXIT("get msgid error");
                }
                break;
            case 'l':
                len = atoi(optarg);
                break;
            case 't':
                type = atoi(optarg);
                break;
            default:
                break;
        }
    }

    if (flg == 1) {
        struct msgbuf* ptr;
        ptr = (struct msgbuf*)malloc(sizeof(long) + len);
        ptr->mtype = type;
        if (msgsnd(msgid, ptr, len, IPC_NOWAIT) < 0) {
            ERR_EXIT("msgsnd error");
        }
    }
    if (flg == 2) {
        struct msgbuf* ptr;
        ptr = (struct msgbuf*)malloc(sizeof(long) + MSGMAX);
        int n;
        if ((n = msgrcv(msgid, ptr, MSGMAX, type, IPC_NOWAIT)) < 0) {
            ERR_EXIT("msgsnd error");
        }

        printf("read %d bytes, type = %ld\n", n, ptr->mtype);
    }

    return 0;
}
