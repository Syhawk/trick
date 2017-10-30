/*
 *	1. int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg);
 *	2. void free(void *ptr);
 *	3. int clock_gettime(clockid_t clk_id, struct timespec *tp);
 *	4. int pthread_detach(pthread_t thread);
 * */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>

#include "posix_threadpool.h"

void* thread_run(void* arg) {
	printf("thread 0x%0x is strating...\n", (int)pthread_self());
	threadpool_t* pool = (threadpool_t*)arg;
	struct timespec abstime;
	int timedout = 0;
	while (1) {
		condition_lock(&(pool->ready));
		++pool->idle;
		// Wait task or threadpool destroy notice.
		while (pool->head == NULL && pool->quit == 0) {
			printf("thread 0x%0x is waiting...\n", (int)pthread_self());
			clock_gettime(CLOCK_REALTIME, &abstime);
			abstime.tv_sec += 2;
			int status;
			status = condition_timedwait(&(pool->ready), &abstime);
			if (status == ETIMEDOUT) {
				printf("thread 0x%0x is timewaited\n", (int)pthread_self());
				timedout = 1;
				break;
			}
		}

		// Process task.
		--pool->idle;
		if (pool->head != NULL) {
			// Get task.
			task_t* ptask = pool->head;
			pool->head = ptask->next;
			// Run task need time, so unlock first, threadpool can add task into tasklist.
			condition_unlock(&(pool->ready));
			ptask->run(ptask->arg);
			free(ptask);
			condition_lock(&(pool->ready));
		}

		// Threadpool destroy notice.
		if (pool->quit == 1 && pool->head == NULL) {
			--pool->counter;
			if (pool->counter == 0) {
				condition_signal(&(pool->ready));
			}
			condition_unlock(&(pool->ready));
			printf("thread 0x%0x exited\n", (int)pthread_self());
			break;
		}

		// Thread timedwait notice.
		if (timedout == 1 && pool->head == NULL) {
			--pool->counter;
			condition_unlock(&(pool->ready));
			printf("thread 0x%0x exited\n", (int)pthread_self());
			break;
		}

		condition_unlock(&(pool->ready));
	}

	return NULL;
}


int threadpool_init(threadpool_t* pool, int threads) {
	int status;
	// THreadpool attribute init.
	status = condition_init(&(pool->ready));
	pool->head = NULL;
	pool->tail = NULL;
	pool->counter = 0;
	pool->idle = 0;
	pool->max_threads = threads;
	pool->quit = 0;

	return status;
}

int threadpool_task_add(threadpool_t* pool, void*(*run)(void* arg), void* arg) {
	if (pool->quit == 1) {
		return 1;
	}

	task_t* ptask = (task_t*)malloc(sizeof(task_t));
	ptask->run = run;
	ptask->arg = arg;
	ptask->next = NULL;
	condition_lock(&(pool->ready));
	// Add new task to task list.
	if (pool->head == NULL) {
		pool->head = ptask;
	} else {
		pool->tail->next = ptask;
	}
	pool->tail = ptask;

	// Check idle thread.
	if (pool->idle > 0) {
		// Wakeup waiting thread.
		condition_signal(&(pool->ready));
	} else if (pool->counter < pool->max_threads) {
		// Create new thread.
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		if (pool->counter == 1) {
			pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		}
		pthread_t tid;
		pthread_create(&tid, &attr, thread_run, pool);
		pthread_attr_destroy(&attr);
		++pool->counter;
	}
	condition_unlock(&(pool->ready));

	return 0;
}

int threadpool_destroy(threadpool_t* pool) {
	if (pool->quit == 1) {
		return 0;
	}

	condition_lock(&(pool->ready));
	pool->quit = 1;
	// Has runing thread or waiting thread.
	if (pool->counter > 0) {
		// Signal to waiting thread.
		if (pool->idle > 0) {
			condition_broadcast(&(pool->ready));
		}

		// Wait runing thread.
		while (pool->counter > 0) {
			condition_wait(&(pool->ready));
		}
	}

	condition_unlock(&(pool->ready));
	condition_destroy(&(pool->ready));

	return 0;
}
