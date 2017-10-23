/*
*   1. int shmget(key_t key, size_t size, int shmflg);
*   2. void *shmat(int shmid, const void *shmaddr, int shmflg);
*   3. int shmdt(const void *shmaddr);
*   4. int shmctl(int shmid, int cmd, struct shmid_ds *buf);
*
*   ipcs
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "interface.h"
#include "common.h"

struct Node {
    char name[32];
    int type;
};

int main(int argc, char* argv[]) {
    // Create shm.
    int shmid;
    shmid = shmget(1234, sizeof(struct Node), IPC_CREAT | 0666);
    if (shmid == -1) {
        ERR_EXIT("shm create error");
    }

    // Map memory.
    struct Node* ptr;
    ptr = (struct Node*)shmat(shmid, NULL, 0);
    if ((void*)ptr == (void*)-1) {
        ERR_EXIT("shmat error");
    }

    // Write data.
    memcpy(ptr->name, "tty1", 4);
    ptr->type = 20;

    while (memcmp(ptr, "exit", 4) != 0);

    // Delete map.
    shmdt(ptr);

    // Delete shm.
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}
