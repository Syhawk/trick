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
	st.name[0] = 'A';
	for (int i = 0; i < 5; ++i) {
		st.age = 20 + i;
		shmfifo_put(fifo, &st);
		st.name[0] = st.name[0] + 1;
		printf("send %d ok.\n", i);
	}

	return 0;
}
