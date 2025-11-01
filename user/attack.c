#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/riscv.h"

#define MEMSIZE PGSIZE*8*8
#define PREFIX "This may help."

int
main(int argc, char *argv[])
{
    // Your code here.
    char *buf = sbrk(MEMSIZE);
    int flag = 0;
    for (int i = 0; i < MEMSIZE; i++) {
        if (buf[i] == PREFIX[0] && memcmp(buf + i, PREFIX, sizeof PREFIX) == 0) {
            if (flag == 0) {
                flag = 1;
                i += sizeof PREFIX;
                continue;
            }
            printf("%s\n", buf + i + 16);
            exit(0);
        }
    }

    exit(1);
}
