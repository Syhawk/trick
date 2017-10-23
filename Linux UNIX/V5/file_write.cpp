/*
*   1. int open(const char *pathname, int flags, mode_t mode);
*   2. off_t lseek(int fd, off_t offset, int whence);
*   3. void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
*   4. int munmap(void *addr, size_t length);
*   5. int close(int fd);
*   6. int msync(void *addr, size_t length, int flags);
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
    int fd = open(argv[1], O_CREAT | O_RDWR | O_TRUNC, 0666);
    if (fd == -1) {
        ERR_EXIT("file open error");
    }
    
    // Alloc sizeof(Node) * 5 bytes.
    lseek(fd, sizeof(struct Node) * 5 - 1, SEEK_SET);
    write(fd, "", 1);

    // Create map.
    struct Node* ptr;
    ptr = (struct Node*)mmap(NULL, sizeof(struct Node) * 5, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == NULL) {
        ERR_EXIT("create map error.");
    }

    // Data write into.
    char name[4] = {"nm1"};
    for (int i = 0; i < 5; ++i) {
        name[2] = char('0' + i + 1);
        memcpy((ptr + i)->name, name, sizeof(name));
        (ptr + i)->type = i + 1;
    }
    puts("file init success.");

    // Delete map.
    munmap(ptr, sizeof(struct Node) * 5);

    // Close fd.
    close(fd);
    
    puts("exit success.");
    
    return 0;
}
