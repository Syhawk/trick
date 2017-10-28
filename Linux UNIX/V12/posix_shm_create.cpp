/*
 *	1. int shm_open(const char *name, int oflag, mode_t mode);
 *	2. int close(int fd);
 *	3. int ftruncate(int fd, off_t length);
 *	4. int fstat(int fd, struct stat *buf);
 *
 *	/dev/shm/
 *	umask
 * */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

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
	int shmid;
	shmid = shm_open("/test", O_CREAT | O_RDWR, 0666);
	if (shmid == -1) {
		ERR_EXIT("shm create error");
	}
	printf("shm create success\n");

	int ret;
	ret = ftruncate(shmid, sizeof(struct Student));
	if (ret == -1) {
		ERR_EXIT("ftruncate shm error");
	}
	printf("modify shm size success\n");

	struct stat buf;
	ret = fstat(shmid, &buf);
	if (ret == -1) {
		ERR_EXIT("get stat error");
	}
	printf("size is %ld, mode = %o\n", buf.st_size, buf.st_mode & 0777);

	close(shmid);

	return 0;
}
