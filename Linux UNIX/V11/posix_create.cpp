/*
 *	1. mqd_t mq_open(const char *name, int oflag);
 *	2. mqd_t mq_open(const char *name, int oflag, mode_t mode, struct mq_attr* attr);
 *	3. int mq_close(mqd_t mqdes);
 *
 *	-lrt
 *	man 7 mq_overview
 *	mkdir /dev/mqueue
 *	mount -t mqueue none /dev/mqueue
 * */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#define ERR_EXIT(m) \
	do { \
		perror(m); \
		exit(EXIT_FAILURE); \
	} while(0)

int main(int argc, char* argv[]) {
	mqd_t mqid;
	mqid = mq_open("/test", O_CREAT | O_RDWR, 0666, NULL);
	if (mqid == (mqd_t)-1) {
		ERR_EXIT("message queue create error");
	}

	printf("message queue create success\n");
	
	mq_close(mqid);

	return 0;
}
