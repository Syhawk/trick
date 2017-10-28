/*
 *	1. mqd_t mq_open(const char *name, int oflag);
 *	2. mqd_t mq_open(const char *name, int oflag, mode_t mode, struct mq_attr* attr);
 *	3. int mq_close(mqd_t mqdes);
 *	4. int mq_getattr(mqd_t mqdes, struct mq_attr *attr);
 *	5. int mq_setattr(mqd_t mqdes, struct mq_attr *newattr, struct mq_attr *oldattr);
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
	mqid = mq_open("/test", O_RDONLY);
	if (mqid == (mqd_t)-1) {
		ERR_EXIT("message queue open error");
	}
	printf("message queue open success\n");

	struct mq_attr attr;
	int ret;
	ret = mq_getattr(mqid, &attr);
	if (ret == -1) {
		ERR_EXIT("get message queue attr error");
	}
	printf("Max message num is %ld\n", attr.mq_maxmsg);
	printf("Max message size is %ld\n", attr.mq_msgsize);
	printf("current message num is %ld\n", attr.mq_curmsgs);

	mq_close(mqid);

	return 0;
}
