#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "posix_threadpool.h"

#define ERR_EXIT(m) \
	do { \
		perror(m); \
		exit(EXIT_FAILURE); \
	} while(0)

void* run(void* arg) {
	printf("thread 0x%0x is working on task %d\n", (int)pthread_self(), *(int*)arg);
	sleep(1);
	free(arg);

	return NULL;
}

int main(int argc, char* argv[]) {
	threadpool_t pool;
	int threads = 3;
	int ret;

	ret = threadpool_init(&pool, threads);
	if (ret != 0) {
		ERR_EXIT("threadpool init error");
	}

	for (int i = 0; i < 10; ++i) {
		int* arg = (int*)malloc(sizeof(int));
		*arg = i;

		ret = threadpool_task_add(&pool, run, arg);
		if (ret != 0) {
			ERR_EXIT("threadpool add task error");
		}
	}

	threadpool_destroy(&pool);

	return 0;
}
