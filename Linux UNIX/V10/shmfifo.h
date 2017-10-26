#ifndef SHM_HEAD_H
#define SHM_HEAD_H

#include <stdio.h>
#include <stdlib.h>

typedef struct shmfifo shmfifo_t;
typedef struct shmhead shmhead_t;

struct shmhead {
	unsigned int block_size;	// block size.
	unsigned int block_num;		// block num.
	unsigned int rd_index;		// read index.
	unsigned int wr_index;		// write index.
};

struct shmfifo {
	shmhead_t* p_shmhead;		// shared memory head pointer.
	char* p_payload;			// start index of valid load.

	int shmid;					// shared memory id.
	int sem_mutex;				// mutex signal.
	int sem_full;				// shared full memory signal.
	int sem_empty;				// shared empty memory signal.
};

shmfifo_t* shmfifo_init(int key, int block_size, int block_num);
void shmfifo_put(shmfifo_t* fifo, const void* buf);
void shmfifo_get(const shmfifo_t* fifo, void* buf);
void shmfifo_destroy(shmfifo_t* fifo);

#endif // SHM_HEAD_H
