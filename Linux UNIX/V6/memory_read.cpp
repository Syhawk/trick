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
    shmid = shmget(1234, 0, 0);
    if (shmid == -1) {
        ERR_EXIT("shm create error");
    }

    // Map memory.
    struct Node* ptr;
    ptr = (struct Node*)shmat(shmid, NULL, 0);
    if ((void*)ptr == (void*)-1) {
        ERR_EXIT("shmat error");
    }

    // Read data.
    printf("name = %s, type = %d\n", ptr->name, ptr->type);
    memcpy(ptr, "exit", 4);

    // Delete map.
    shmdt(ptr);

    return 0;
}
