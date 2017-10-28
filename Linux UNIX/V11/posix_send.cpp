/*
 *	1. mqd_t mq_open(const char *name, int oflag);
 *	2. mqd_t mq_open(const char *name, int oflag, mode_t mode, struct mq_attr* attr);
 *	3. int mq_close(mqd_t mqdes);
 *	4. int mq_send(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned msg_prio);
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

struct Student {
	char name[32];
	int age;
};

int main(int argc, char* argv[]) {
	if (argc != 2) {
		fprintf(stderr, "Usage %s <prio>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	mqd_t mqid;
	mqid = mq_open("/test", O_RDWR);
	if (mqid == (mqd_t)-1) {
		ERR_EXIT("message queue open error");
	}
	printf("message queue open success\n");

	Student st;
	strcpy(st.name, "st1");
	st.age = 20;
	unsigned msg_prio = atoi(argv[1]);
	int ret;
	ret = mq_send(mqid, (char*)&st, sizeof(st), msg_prio);
	if (ret == -1) {
		ERR_EXIT("message send error");
	}
	printf("message send success\n");

	mq_close(mqid);

	return 0;
}
