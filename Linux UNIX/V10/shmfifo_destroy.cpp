/*
 *	gdb file
 *	r
 *	p val
 * */

#include <string.h>

#include "shmfifo.h"

struct student {
	char name[32];
	int age;
};


int main(int argc, char* argv[]) {
	shmfifo_t* fifo = shmfifo_init(1234, sizeof(student), 3);
	shmfifo_destroy(fifo);

	return 0;
}
