/*
 *	1. int pthread_cond_init(pthread_cond_t *cond, pthread_condattr_t *cond_attr);
 *	2. int pthread_cond_destroy(pthread_cond_t *cond);
 *	3. int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *mutexattr);
 *	4. int pthread_mutex_destroy(pthread_mutex_t *mutex);
 *	5. int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg);
 *	6. int pthread_join(pthread_t thread, void **retval);
 *	7. int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
 *	8. int pthread_cond_signal(pthread_cond_t *cond);
 *	9. int pthread_mutex_lock(pthread_mutex_t *mutex);
 *	10. int pthread_mutex_unlock(pthread_mutex_t *mutex);
 * */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define ERR_EXIT(m) \
	do { \
		perror(m); \
		exit(EXIT_FAILURE); \
	} while(0)

#define CONSUMER_COUNT 2
#define PRODUCER_COUNT 4
#define BUFFER_SIZE 10

int buffer[BUFFER_SIZE];
unsigned int write_index = 0;
unsigned int read_index = 0;
unsigned int merchandy = 0;

pthread_cond_t cond;
int empty = BUFFER_SIZE;

pthread_mutex_t mutex;
pthread_t threads[CONSUMER_COUNT + PRODUCER_COUNT];

void* produce(void* arg) {
	int no = *((int*)arg);
	free(arg);
	while (1) {
		printf("produce %d wait not full\n", no);

		pthread_mutex_lock(&mutex);
		while (empty == 0);

		printf("begin produce marchandy %d\n", merchandy);
		buffer[write_index] = merchandy;
		write_index = (write_index + 1) % BUFFER_SIZE;
		printf("end produce marchandy %d\n", merchandy++);
	
		if (empty == BUFFER_SIZE) {
			pthread_cond_signal(&cond);
		}
		--empty;
		pthread_mutex_unlock(&mutex);
		sleep(5);
	}

	return NULL;
}

void* consume(void* arg) {
	int no = *((int*)arg);
	free(arg);
	while (1) {
		printf("consume %d wait not empty\n", no);
		pthread_mutex_lock(&mutex);
		while (empty == BUFFER_SIZE) {
			printf("consume %d start a condition...\n", no);
			pthread_cond_wait(&cond, &mutex);
		}

		int good = buffer[read_index];
		buffer[read_index] = -1;
		printf("begin consume marchandy %d\n", good);
		read_index = (read_index + 1) % BUFFER_SIZE;
		printf("end consume marchandy %d\n", good);
		++empty;
	
		pthread_mutex_unlock(&mutex);
		sleep(1);
	}

	return NULL;
}


int main(int argc, char* argv[]) {
	for (int i = 0; i < BUFFER_SIZE; ++i) {
		buffer[i] = -1;
	}

	int ret = 0;
	ret = pthread_cond_init(&cond, NULL);
	if (ret != 0) {
		ERR_EXIT("pthread_cond_init cond error");
	}

	pthread_mutex_init(&mutex, NULL);

	for (int i = 0; i < PRODUCER_COUNT; ++i) {
		int* no = (int*)malloc(sizeof(int));
		*no = i;
		pthread_create(&threads[i], NULL, produce, (void*)no);
	}

	for (int i = 0; i < CONSUMER_COUNT; ++i) {
		int* no = (int*)malloc(sizeof(int));
		*no = i;
		pthread_create(&threads[*no + PRODUCER_COUNT], NULL, consume, (void*)no);
	}

	for (int i = 0; i < PRODUCER_COUNT + CONSUMER_COUNT; ++i) {
		pthread_join(threads[i], NULL);
	}

	pthread_mutex_destroy(&mutex);

	pthread_cond_destroy(&cond);

	return 0;
}
