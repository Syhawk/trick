/*
 *	1. int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *mutexattr);
 *	2. int pthread_mutex_destroy(pthread_mutex_t *mutex);
 *	3. int pthread_mutex_lock(pthread_mutex_t *mutex);
 *	4. int pthread_mutex_unlock(pthread_mutex_t *mutex);
 *	5. int pthread_cond_init(pthread_cond_t *cond, pthread_condattr_t *cond_attr);
 *	6. int pthread_cond_destroy(pthread_cond_t *cond);
 *	7. int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
 *	8. int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime);
 *	9. int pthread_cond_signal(pthread_cond_t *cond);
 *	10. int pthread_cond_broadcast(pthread_cond_t *cond);
 * */

#include "posix_condition.h"

int condition_init(condition_t* cond) {
	int status;
	status = pthread_mutex_init(&(cond->pmutex), NULL);
	if (status != 0) {
		return status;
	}
	
	status = pthread_cond_init(&(cond->pcond), NULL);

	return status;
}

int condition_destroy(condition_t* cond) {
	int status;
	status = pthread_mutex_destroy(&(cond->pmutex));
	if (status != 0) {
		return status;
	}

	status = pthread_cond_destroy(&(cond->pcond));

	return status;
}

int condition_lock(condition_t* cond) {
	return pthread_mutex_lock(&(cond->pmutex));
}
int condition_unlock(condition_t* cond) {
	return pthread_mutex_unlock(&(cond->pmutex));
}

int condition_wait(condition_t* cond) {
	return pthread_cond_wait(&(cond->pcond), &(cond->pmutex));
}

int condition_timedwait(condition_t* cond, const struct timespec* abstime) {
	return pthread_cond_timedwait(&(cond->pcond), &(cond->pmutex), abstime);
}

int condition_signal(condition_t* cond) {
	return pthread_cond_signal(&(cond->pcond));
}

int condition_broadcast(condition_t* cond) {
	return pthread_cond_broadcast(&(cond->pcond));
}
