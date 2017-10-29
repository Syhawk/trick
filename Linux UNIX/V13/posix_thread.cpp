/*
 *	1. int pthread_attr_init(pthread_attr_t *attr);
 *	2. int pthread_attr_destroy(pthread_attr_t *attr);
 *	3. int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate);
 *	4. int pthread_attr_getdetachstate(pthread_attr_t *attr, int *detachstate);
 *	5. int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize);
 *	6. int pthread_attr_getstacksize(pthread_attr_t *attr, size_t *stacksize);
 *	7. int pthread_attr_setguardsize(pthread_attr_t *attr, size_t guardsize);
 *	8. int pthread_attr_getguardsize(pthread_attr_t *attr, size_t *guardsize);
 *	9. int pthread_attr_setscope(pthread_attr_t *attr, int scope);
 *	10. int pthread_attr_getscope(pthread_attr_t *attr, int *scope);
 *	11. int pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy);
 *	12. int pthread_attr_getschedpolicy(pthread_attr_t *attr, int *policy);
 *	13. int pthread_attr_setinheritsched(pthread_attr_t *attr, int inheritsched);
 *	14. int pthread_attr_getinheritsched(pthread_attr_t *attr, int *inheritsched);
 *	15. int pthread_attr_setschedparam(pthread_attr_t *attr, const struct sched_param *param);
 *	16. int pthread_attr_getschedparam(pthread_attr_t *attr, struct sched_param *param);
 *	17. int pthread_setconcurrency(int new_level);
 *	18. int pthread_getconcurrency(void);
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

int main(int argc, char* argv[]) {
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	int ret;

	int state;
	ret = pthread_attr_getdetachstate(&attr, &state);
	if (ret != 0) {
		ERR_EXIT("pthread_attr_getdetachstate error");
	}
	if (state == PTHREAD_CREATE_DETACHED) {
		printf("detachstate:PTHREAD_CREATE_DETACH\n");
	} else if (state == PTHREAD_CREATE_JOINABLE) {
		printf("detachstate:PTHREAD_CREATE_JOINABLE\n");
	}

	size_t size;
	ret = pthread_attr_getstacksize(&attr, &size);
	if (ret != 0) {
		ERR_EXIT("pthread_attr_getstacksize error");
	}
	printf("stacksize = %lu\n", size);

	ret = pthread_attr_getguardsize(&attr, &size);
	if (ret != 0) {
		ERR_EXIT("pthread_attr_getguardsize error");
	}
	printf("guardsize = %lu\n", size);

	int scope;
	ret = pthread_attr_getscope(&attr, &scope);
	if (ret != 0) {
		ERR_EXIT("pthread_attr_getscope error");
	}
	if (scope == PTHREAD_SCOPE_SYSTEM) {
		printf("thread scope:PTHREAD_SCOPE_SYSTEM\n");
	} else if (scope == PTHREAD_SCOPE_PROCESS) {
		printf("thread scope:PTHREAD_SCOPE_PROCESS\n");
	}

	int policy;
	ret = pthread_attr_getschedpolicy(&attr, &policy);
	if (ret != 0) {
		ERR_EXIT("pthread_attr_getschedpolicy error");
	}
	if (policy == SCHED_FIFO) {
		printf("thread schedule policy is:SCHED_FIFO\n");
	} else if (policy == SCHED_RR) {
		printf("thread schedule policy is:SCHED_RR\n");
	} else if (policy == SCHED_OTHER) {
		printf("thread schedule policy is:SCHED_OTHER\n");
	}

	int inheritsched;
	ret = pthread_attr_getinheritsched(&attr, &inheritsched);
	if (ret != 0) {
		ERR_EXIT("pthread_attr_getinheritsched error");
	}
	if (inheritsched == PTHREAD_INHERIT_SCHED) {
		printf("thread inheritsched is:PTHREAD_INHERIT_SCHED\n");
	} else if (inheritsched == PTHREAD_INHERIT_SCHED) {
		printf("thread inheritsched is:PTHREAD_EXPLICIT_SCHED\n");
	}

	struct sched_param param;
	ret = pthread_attr_getschedparam(&attr, &param);
	if (ret != 0) {
		ERR_EXIT("pthread_attr_getschedparam error");
	}
	printf("sched_priority = %d\n", param.sched_priority);

	pthread_attr_destroy(&attr);

	int level;
	level = pthread_getconcurrency();
	printf("currency level = %d\n", level);

	return 0;
}
