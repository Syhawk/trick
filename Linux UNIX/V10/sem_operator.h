#ifndef SEM_OPERATOR_H
#define SEM_OPERATOR_H

/*
*   1. int semget(key_t key, int nsems, int semflg);
*   2. int semctl(int semid, int semnum, int cmd, ...);
*   3. int semop(int semid, struct sembuf *sops, unsigned nsops);
*
*   ipcs, ipcrm -s
*   ps -ef | grep defunct
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>

#define ERR_EXIT(m) \
	do { \
		perror(m);    \
		exit(EXIT_FAILURE); \
	} while (0)

union semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO (Linux-specific) */
};

int sem_create(key_t key, int num);
int sem_create(key_t key);
int sem_open(key_t key);
int sem_setval(int semid, int val);
int sem_getval(int semid);
int sem_destroy(int semid);
int sem_p(int semid);
int sem_v(int semid);

#endif // SEM_OPERATOR_H
