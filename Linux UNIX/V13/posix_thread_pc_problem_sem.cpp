/*
 *	1. int sem_init(sem_t *sem, int pshared, unsigned int value);
 *	2. int sem_destroy(sem_t *sem);
 *	3. int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *mutexattr);
 *	4. int pthread_mutex_destroy(pthread_mutex_t *mutex);
 *	5. int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg);
 *	6. int pthread_join(pthread_t thread, void **retval);
 *	7. int sem_wait(sem_t *sem);
 *	8. int sem_post(sem_t *sem);
 *	9. int pthread_mutex_lock(pthread_mutex_t *mutex);
 *	10. int pthread_mutex_unlock(pthread_mutex_t *mutex);
 * */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define ERR_EXIT(m) \
	do { \
		perror(m); \
		exit(EXIT_FAILURE); \
	} while(0)

#define CONSUMER_COUNT 1
#define PRODUCER_COUNT 5
#define BUFFER_SIZE 10

int buffer[BUFFER_SIZE];
unsigned int write_index = 0;
unsigned int read_index = 0;
unsigned int merchandy = 0;

sem_t sem_full;
sem_t sem_empty;

pthread_mutex_t mutex;
pthread_t threads[CONSUMER_COUNT + PRODUCER_COUNT];


void* produce(void* arg) {
	int no = *((int*)arg);
	while (1) {
		printf("produce %d wait not full\n", no);
		sem_wait(&sem_empty);
		pthread_mutex_lock(&mutex);
		for (unsigned int i = 0; i < BUFFER_SIZE; ++i) {
			printf("%02d ", i);
			if (buffer[i] == -1) {
				printf("%s", "null");
			} else {
				printf("%d", buffer[i]);
			}
			if (i == write_index) {
				printf("<--produce");
			}
			printf("\n");
		}

		printf("begin produce marchandy %d\n", merchandy);
		buffer[write_index] = merchandy;
		write_index = (write_index + 1) % BUFFER_SIZE;
		printf("end produce marchandy %d\n", merchandy++);

		pthread_mutex_unlock(&mutex);
		sem_post(&sem_full);

		sleep(5);
	}

	return NULL;
}

void* consume(void* arg) {
	int no = *((int*)arg);
	while (1) {
		printf("consume %d wait not empty\n", no);
		sem_wait(&sem_full);
		pthread_mutex_lock(&mutex);
	
		for (unsigned int i = 0; i < BUFFER_SIZE; ++i) {
			printf("%02d ", i);
			if (buffer[i] == -1) {
				printf("%s", "null");
			} else {
				printf("%d", buffer[i]);
			}
			if (i == read_index) {
				printf("<--consume");
			}
			printf("\n");
		}

		int good = buffer[read_index];
		buffer[read_index] = -1;
		printf("begin consume marchandy %d\n", good);
		read_index = (read_index + 1) % BUFFER_SIZE;
		printf("end consume marchandy %d\n", good);
	
		pthread_mutex_unlock(&mutex);
		sem_post(&sem_empty);

		sleep(1);
	}

	return NULL;
}


int main(int argc, char* argv[]) {
	for (int i = 0; i < BUFFER_SIZE; ++i) {
		buffer[i] = -1;
	}

	int ret = 0;
	ret = sem_init(&sem_full, 0, 0);
	if (ret != 0) {
		ERR_EXIT("sme_init sem_full error");
	}
	ret = sem_init(&sem_empty, 0, BUFFER_SIZE);
	if (ret != 0) {
		ERR_EXIT("sme_init sem_empty error");
	}

	pthread_mutex_init(&mutex, NULL);

	for (int i = 0; i < PRODUCER_COUNT; ++i) {
		int no = i;
		pthread_create(&threads[i], NULL, produce, (void*)&no);
	}

	for (int i = 0; i < CONSUMER_COUNT; ++i) {
		int no = i;
		pthread_create(&threads[no + PRODUCER_COUNT], NULL, consume, (void*)&no);
	}

	for (int i = 0; i < PRODUCER_COUNT + CONSUMER_COUNT; ++i) {
		pthread_join(threads[i], NULL);
	}

	pthread_mutex_destroy(&mutex);

	sem_destroy(&sem_empty);
	sem_destroy(&sem_full);


	return 0;
}
