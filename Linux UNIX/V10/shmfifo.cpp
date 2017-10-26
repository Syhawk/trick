#include <assert.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <string.h>
#include <unistd.h>

#include "shmfifo.h"
#include "sem_operator.h"

shmfifo_t* shmfifo_init(int key, int block_size, int block_num);
void shmfifo_put(shmfifo_t* fifo, const void* buf);
void shmfifo_get(const shmfifo_t* fifo, void* buf);
void shmfifo_destroy(shmfifo_t* fifo);

shmfifo_t* shmfifo_init(int key, int block_size, int block_num) {
	// Create shmfifo_t pointer object and init.
	shmfifo_t* fifo = (shmfifo_t*)malloc(sizeof(shmfifo_t));
	assert(fifo != NULL);
	memset(fifo, 0, sizeof(shmfifo_t));

	// Check shm(key) has existed.
	fifo->shmid = shmget(key, 0, 0);
	if (fifo->shmid == -1) {
		// Create new shared memory.
		int size = sizeof(shmfifo_t) + block_size * block_num;
		fifo->shmid = shmget(key, size, IPC_CREAT | 0666);
		if (fifo->shmid == -1) {
			ERR_EXIT("shmget error");
		}

		// Init object fifo semaphore attribute.
		fifo->sem_mutex = sem_create(key);
		fifo->sem_full = sem_create(key + 1);
		fifo->sem_empty = sem_create(key + 2);

		// Semaphore init.
		sem_setval(fifo->sem_mutex, 1);
		sem_setval(fifo->sem_full, 0);
		sem_setval(fifo->sem_empty, block_num);
	} else {
		// Init object fifo semaphore attribute.
		fifo->sem_mutex = sem_open(key);
		fifo->sem_full = sem_open(key + 1);
		fifo->sem_empty = sem_open(key + 2);
	}

	// Init object fifo shared memory attribute.
	fifo->p_shmhead = (shmhead_t*)shmat(fifo->shmid, NULL, 0);
	if ((void*)(fifo->p_shmhead) == (void*)-1) {
		ERR_EXIT("shmat error");
	}

	// Init p_payload.
	fifo->p_payload = (char*)(fifo->p_shmhead + 1);

	// Init shmhead_t point object.
	fifo->p_shmhead->block_size = block_size;
	fifo->p_shmhead->block_num = block_num;
	fifo->p_shmhead->rd_index = 0;
	fifo->p_shmhead->wr_index = 0;

	return fifo;
}

void shmfifo_put(shmfifo_t* fifo, const void* buf) {
	sem_p(fifo->sem_empty);
	sem_p(fifo->sem_mutex);

	shmhead_t* p_shmhead = fifo->p_shmhead;
	memcpy(fifo->p_payload + p_shmhead->block_size * p_shmhead->wr_index, buf, p_shmhead->block_size);
	p_shmhead->wr_index = (p_shmhead->wr_index + 1) % p_shmhead->block_num;

	sem_v(fifo->sem_mutex);
	sem_v(fifo->sem_full);
}

void shmfifo_get(const shmfifo_t* fifo, void* buf) {
	sem_p(fifo->sem_full);
	sem_p(fifo->sem_mutex);

	shmhead_t* p_shmhead = fifo->p_shmhead;
	memcpy(buf, fifo->p_payload + p_shmhead->block_size * p_shmhead->rd_index, p_shmhead->block_size);
	p_shmhead->rd_index = (p_shmhead->rd_index + 1) % p_shmhead->block_num;

	sem_v(fifo->sem_mutex);
	sem_v(fifo->sem_empty);
}

void shmfifo_destroy(shmfifo_t* fifo) {
	// Destroy semaphore.
	sem_destroy(fifo->sem_mutex);
	sem_destroy(fifo->sem_full);
	sem_destroy(fifo->sem_empty);

	// Delete shared memory.
	shmdt(fifo->p_shmhead);
	shmctl(fifo->shmid, IPC_RMID, 0);

	// Delete shmfifo_t pointer.
	free(fifo);
}

