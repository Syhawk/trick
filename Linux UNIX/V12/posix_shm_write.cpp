/*
 *	1. int shm_open(const char *name, int oflag, mode_t mode);
 *	2. int close(int fd);
 *	3. int ftruncate(int fd, off_t length);
 *	4. int fstat(int fd, struct stat *buf);
 *	5. void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
 *	6. int munmap(void *addr, size_t length);
 *
 *	/dev/shm/
 *	umask
 *	od -c <filename>
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
	shmid = shm_open("/test", O_RDWR, 0666);
	if (shmid == -1) {
		ERR_EXIT("shm open error");
	}
	printf("shm open success\n");

	int ret;
	struct stat buf;
	ret = fstat(shmid, &buf);
	if (ret == -1) {
		ERR_EXIT("get stat error");
	}

	Student* st;
	st = (Student*)mmap(NULL, buf.st_size, PROT_WRITE, MAP_SHARED, shmid, 0);
	if (st == MAP_FAILED) {
		ERR_EXIT("map shm error");
	}

	strcpy(st->name, "test");
	st->age = 4;

	munmap(st, sizeof(struct Student));
	close(shmid);

	return 0;
}
