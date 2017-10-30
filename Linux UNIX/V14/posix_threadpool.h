#ifndef POSIX_THREADPOOL_H
#define POSIX_THREADPOOL_H

#include <pthread.h>
#include "posix_condition.h"

// Task struct for desc task detail info.
typedef struct task {
	void *(*run)(void* arg);
	void *arg;
	struct task* next;
}task_t;

// THreadpool struct.
typedef struct threadpool {
	condition_t ready;			// Task parepare or threadpool destroy notice.
	task_t* head;				// Task list head.
	task_t* tail;				// Task list tail.
	int counter;				// Threadpool current thread nums.
	int idle;					// Threadpool current waiting thread nums.
	int max_threads;			// Threadpool maximum capacity of thread nums.
	int quit;					// Threadpool destroy if quit value is 1.
}threadpool_t;

// Init threadpool.
int threadpool_init(threadpool_t* pool, int threads);
// Threadpool add task.
int threadpool_task_add(threadpool_t* pool, void*(*run)(void* arg), void* arg);
// Threadpool destroy.
int threadpool_destroy(threadpool_t* pool);

#endif	// POSIX_THREADPOOL_H
