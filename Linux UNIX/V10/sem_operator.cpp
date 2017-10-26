#include "sem_operator.h"

int sem_create(key_t key, int num) {
    int semid;
    semid = semget(key, num, IPC_CREAT | IPC_EXCL | 0666);
    if (semid == -1) {
        ERR_EXIT("sem create error");
    }

    return semid;
}

int sem_create(key_t key) {
	return sem_create(key, 1);
}

int sem_open(key_t key) {
    int semid;
    semid = semget(key, 0, 0);
    if (semid == -1) {
        ERR_EXIT("sem open error");
    }

    return semid;
}

int sem_setval(int semid, int val) {
	union semun su;
	su.val = val;
    int ret = semctl(semid, 0, SETVAL, su);
    if (ret == -1) {
        ERR_EXIT("sem setval error");
    }

    return 0;
}

int sem_getval(int semid) {
    int ret = semctl(semid, 0, GETVAL, 0);
    if (ret == -1) {
        ERR_EXIT("sem setval error");
    }

    return ret;
}

int sem_destroy(int semid) {
    int ret;
    ret = semctl(semid, 0, IPC_RMID, 0);
    if (ret == -1) {
        ERR_EXIT("sem delete error");
    }

    return ret;
}

int sem_p(int semid) {
    struct sembuf buf = {0, -1, 0};
    int ret = semop(semid, &buf, 1);
    if (ret == -1) {
        ERR_EXIT("sem p error");
    }

    return ret;
}

int sem_v(int semid) {
    struct sembuf buf = {0, 1, 0};

    int ret = semop(semid, &buf, 1);
    if (ret == -1) {
        ERR_EXIT("sem v error");
    }

    return ret;
}
