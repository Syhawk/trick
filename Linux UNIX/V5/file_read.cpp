/*
*   1. int open(const char *pathname, int flags, mode_t mode);
*   2. void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
*   3. int munmap(void *addr, size_t length);
*   4. int close(int fd);
*
*   od -c filename
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>

#include "interface.h"
#include "common.h"

struct Node {
    char name[4];
    int type;
};

int main(int argc, char* argv[]) {
    // Argv num.
    if (argc != 2) {
        fprintf(stderr, "Usage %s <file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    // Open file.
    int fd = open(argv[1], O_RDWR);
    if (fd == -1) {
        ERR_EXIT("file open error");
    }

    // Create map.
    struct Node* ptr;
    ptr = (struct Node*)mmap(NULL, sizeof(struct Node) * 5, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == NULL) {
        ERR_EXIT("create map error.");
    }

    // Data read.
    for (int i = 0; i < 5; ++i) {
        printf("name = %s, type = %d\n", (ptr + i)->name, (ptr + i)->type);
    }

    // Delete map.
    munmap(ptr, sizeof(struct Node) * 5);

    // Close fd.
    close(fd);
    
    return 0;
}
