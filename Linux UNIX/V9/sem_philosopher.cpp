/*
*   1. int semget(key_t key, int nsems, int semflg);
*   2. int semctl(int semid, int semnum, int cmd, ...);
*   3. int semop(int semid, struct sembuf *sops, unsigned nsops);
*   4. pid_t fork(void);
*   5. key_t ftok(const char *pathname, int proj_id);
*   6. pid_t getpid(void);
*   7. pid_t wait(int *status);
*
*   ipcs, ipcrm -s
*   ps -ef | grep defunct
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <time.h>
#include <sys/wait.h>
#include <signal.h>

#include "interface.h"
#include "common.h"


union semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO (Linux-specific) */
};


#define DELAY (rand() % 5 + 1)

int sem_create(key_t key, int num) {
    int semid;
    semid = semget(key, num, IPC_CREAT | 0666);
    if (semid == -1) {
        ERR_EXIT("sem create error");
    }

    return semid;
}

int sem_setval(int semid, int num, int cmd, union semun su) {
    int ret = semctl(semid, num, cmd, su);
    if (ret == -1) {
        ERR_EXIT("sem setval error");
    }

    return 0;
}

int sem_delete(int semid) {
    int ret;
    ret = semctl(semid, 0, IPC_RMID, 0);
    if (ret == -1) {
        ERR_EXIT("sem delete error");
    }

    return ret;
}

int sem_p(int semid, int no) {
    int left = no;
    int right = (no + 1) % 5;
    struct sembuf buf[2] = {{left, -1, 0}, {right, -1, 0}};

    int ret = semop(semid, buf, 2);
    if (ret == -1) {
        ERR_EXIT("sem p error");
    }

    return ret;
}

int sem_v(int semid, int no) {
    int left = no;
    int right = (no + 1) % 5;
    struct sembuf buf[2] = {{left, 1, 0}, {right, 1, 0}};

    int ret = semop(semid, buf, 2);
    if (ret == -1) {
        ERR_EXIT("sem v error");
    }

    return ret;
}

void philosopher(int semid, int no) {
    srand(getpid());
    for (int i = 0; i < 3; ++i) {
        printf("%d is thinking...\n", no);
        sleep(DELAY);
        printf("%d is hungry\n", no);
        sem_p(semid, no);
        printf("%d is eating...\n", no);
        sleep(DELAY);
        sem_v(semid, no);
    }
}

int main(int argc, char* argv[]) {
    int semid = sem_create(IPC_PRIVATE, 5);

    union semun su;
    su.val = 1;
    for (int i = 0; i < 5; ++i) {
        sem_setval(semid, i, SETVAL, su);
    }

    pid_t pid;
    for (int i = 1; i < 5; ++i) {
        pid = fork();
        if (pid == -1) {
            ERR_EXIT("process create error");
        }
        if (pid == 0) {
            philosopher(semid, i);
            exit(EXIT_SUCCESS);
        }
    }

    philosopher(semid, 0);
    for (int i = 0; i < 4; ++i) {
        wait(NULL);
    }
    sem_delete(semid);

    return 0;
}
