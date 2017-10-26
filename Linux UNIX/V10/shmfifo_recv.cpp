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

	student st;
	memset(&st, 0, sizeof(st));
	for (int i = 0; i < 5; ++i) {
		shmfifo_get(fifo, &st);
		printf("recv name = %s, age = %d ok.\n", st.name, st.age);
	}

	shmfifo_destroy(fifo);

	return 0;
}
