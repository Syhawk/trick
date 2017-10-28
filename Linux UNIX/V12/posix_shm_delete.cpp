/*
 *	1. int shm_open(const char *name, int oflag, mode_t mode);
 *	2. int shm_unlink(const char *name);
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

int main(int argc, char* argv[]) {
	int ret;
	ret = shm_unlink("/test");
	if (ret == -1) {
		ERR_EXIT("unlink shm error");
	}

	return 0;
}
