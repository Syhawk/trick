/*
 *	1. mqd_t mq_open(const char *name, int oflag);
 *	2. mqd_t mq_open(const char *name, int oflag, mode_t mode, struct mq_attr* attr);
 *	3. int mq_close(mqd_t mqdes);
 *	4. int mq_getattr(mqd_t mqdes, struct mq_attr *attr);
 *	5. ssize_t mq_receive(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned *msg_prio);
 *	6. int mq_notify(mqd_t mqdes, const struct sigevent *sevp);
 *	7. sighandler_t signal(int signum, sighandler_t handler);
 *
 *	-lrt
 *	man 7 mq_overview
 *	mkdir /dev/mqueue
 *	mount -t mqueue none /dev/mqueue
 *	man 7 sigevent
 * */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <signal.h>
#include <unistd.h>

#define ERR_EXIT(m) \
	do { \
		perror(m); \
		exit(EXIT_FAILURE); \
	} while(0)

struct Student {
	char name[32];
	int age;
};

mqd_t mqid;
struct sigevent sigev;
size_t msg_len;

void handle_sigusr1(int sig) {
	mq_notify(mqid, &sigev);

	Student st;
	unsigned msg_prio;
	ssize_t ret;
	ret = mq_receive(mqid, (char*)&st, msg_len, &msg_prio);
	if (ret == -1) {
		ERR_EXIT("message recv error");
	}
	printf("message recv success\n");
	printf("Student name is %s, age is %d, prio is %u, recv bytes is %ld\n", st.name, st.age, msg_prio, ret);
}

int main(int argc, char* argv[]) {
	signal(SIGUSR1, handle_sigusr1);

	mqid = mq_open("/test", O_RDONLY);
	if (mqid == (mqd_t)-1) {
		ERR_EXIT("message queue open error");
	}
	printf("message queue open success\n");

	struct mq_attr attr;
	if (mq_getattr(mqid, &attr) < 0) {
		ERR_EXIT("message queue get attr error");
	}
	msg_len = attr.mq_msgsize;

	sigev.sigev_notify = SIGEV_SIGNAL;
	sigev.sigev_signo = SIGUSR1;
	mq_notify(mqid, &sigev);

	while(1) {
		pause();
	}

	mq_close(mqid);

	return 0;
}
