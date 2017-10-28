/*
 *	1. int mq_unlink(const char *name);
 *
 *	-lrt
 * */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <mqueue.h>

#define ERR_EXIT(m) \
	do { \
		perror(m); \
		exit(EXIT_FAILURE); \
	} while(0)

int main(int argc, char* argv[]) {
	int ret = mq_unlink("/test");
	if (ret == -1) {
		ERR_EXIT("message queue delete error");
	}

	printf("message queue delete success\n");
	
	return 0;
}
