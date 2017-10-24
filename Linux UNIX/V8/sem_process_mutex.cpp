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

#include "interface.h"
#include "common.h"


union semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO (Linux-specific) */
};


int sem_create(key_t key) {
    int semid;
    semid = semget(key, 1, IPC_CREAT | IPC_EXCL | 0666);
    if (semid == -1) {
        ERR_EXIT("sem create error");
    }

    return semid;
}

int sem_open(key_t key) {
    int semid;
    semid = semget(key, 0, 0);
    if (semid == -1) {
        ERR_EXIT("sem get error");
    }

    return semid;
}

int sem_setval(int semid, int val) {
    union semun un;
    un.val = val;
    int ret = semctl(semid, 0, SETVAL, un);
    if (ret == -1) {
        ERR_EXIT("sem setval error");
    }

    return 0;
}

int sem_getval(int semid) {
    int val;
    val = semctl(semid, 0, GETVAL, 0);
    if (val == -1) {
        ERR_EXIT("sem get val error");
    }

    return val;
}

int sem_delete(int semid) {
    int ret;
    ret = semctl(semid, 0, IPC_RMID, 0);
    if (ret == -1) {
        ERR_EXIT("sem delete error");
    }

    return ret;
}

int sem_p(int semid) {
    struct sembuf sb = {0, -1, 0};
    int ret;
    ret = semop(semid, &sb, 1);
    if (ret == -1) {
        ERR_EXIT("sem p error");
    }

    return ret;
}

int sem_v(int semid) {
    struct sembuf sb = {0, 1, 0};
    int ret;
    ret = semop(semid, &sb, 1);
    if (ret == -1) {
        ERR_EXIT("sem v error");
    }

    return ret;
}

void show(int semid, char ch) {
    int pause_time;
    srand(getpid());
    for (int i = 0; i < 10; ++i) {
        sem_p(semid);
        printf("%c", ch);
        fflush(stdout);
        pause_time = rand() % 3;
        sleep(pause_time);
        printf("%c", ch);
        fflush(stdout);
        sem_v(semid);
        pause_time = rand() % 2;
        sleep(pause_time);
    }
}

int main(int argc, char* argv[]) {
    key_t key = ftok(".", 's');
    int semid = sem_create(IPC_PRIVATE);
    sem_setval(semid, 0);

    pid_t pid;
    pid = fork();
    if (pid == -1) {
        ERR_EXIT("create pid error");
    }
    if (pid > 0) {
        sem_setval(semid, 1);
        show(semid, 'O');
        wait(NULL);
        sem_delete(semid);
        puts("");
        fflush(stdout);
    } else {
        show(semid, 'X');
    }

    return 0;
}
