/*
*   1. int semget(key_t key, int nsems, int semflg);
*   2. int semctl(int semid, int semnum, int cmd, ...);
*   3. int semop(int semid, struct sembuf *sops, unsigned nsops);
*   4. int getopt(int argc, char * const argv[], const char *optstring);
*   5. key_t ftok(const char *pathname, int proj_id);
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

int sem_getmode(int semid) {
    union semun su;
    struct semid_ds sem;
    su.buf = &sem;
    
    int ret = semctl(semid, 0, IPC_STAT, su);
    if (ret == -1) {
        ERR_EXIT("sem stat error");
    }
    printf("current permission is %o\n", su.buf->sem_perm.mode);

    return ret;
}

int sem_setmode(int semid, char* mode) {
    union semun su;
    struct semid_ds sem;
    su.buf = &sem;

    int ret = semctl(semid, 0, IPC_STAT, su);
    if (ret == -1) {
        ERR_EXIT("sem stat error");
    }
    printf("current premission is %o\n", su.buf->sem_perm.mode);

    sscanf(mode, "%o", (unsigned int*)&(su.buf->sem_perm.mode));
//    su.buf->sem_perm.mode = mode;
    ret = semctl(semid, 0, IPC_SET, su);
    if (ret == -1) {
        ERR_EXIT("sem mode set error");
    }
    printf("permission updated...\n");

    return ret;
}

void usage() {
    fprintf(stderr, "usage:\n");
    fprintf(stderr, "semtool -c\n");
    fprintf(stderr, "semtool -o\n");
    fprintf(stderr, "semtool -s <val>\n");
    fprintf(stderr, "semtool -g\n");
    fprintf(stderr, "semtool -d\n");
    fprintf(stderr, "semtool -p\n");
    fprintf(stderr, "semtool -v\n");
    fprintf(stderr, "semtool -f\n");
    fprintf(stderr, "semtool -m <mode>\n");
}

int main(int argc, char* argv[]) {
    int opt;
    opt = getopt(argc, argv, "cos:gdpvfm:");
    if (opt == '?') {
        exit(EXIT_FAILURE);
    }
    if (opt == -1) {
        usage();
        exit(EXIT_FAILURE);
    }
    
    key_t key = ftok(".", 's');
    int semid;
    switch(opt) {
        case 'c':
            sem_create(key);
            break;
        case 'o':
            semid = sem_open(key);
            break;
        case 's':
            semid = sem_open(key);
            sem_setval(semid, atoi(optarg));
            break;
        case 'g':
            semid = sem_open(key);
            sem_getval(semid);
            break;
        case 'd':
            semid = sem_open(key);
            sem_delete(semid);
            break;
        case 'p':
            semid = sem_open(key);
            sem_p(semid);
            break;
        case 'v':
            semid = sem_open(key);
            sem_v(semid);
            break;
        case 'f':
            semid = sem_open(key);
            sem_getmode(semid);
            break;
        case 'm':
            semid = sem_open(key);
            sem_setmode(semid, optarg);
            break;
        default:
            break;
    }

    return 0;
}
