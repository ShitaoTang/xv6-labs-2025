#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(2, "Usage: sleep <sec>\n");
        exit(1);
    }

    int ticks = atoi(argv[1]);
    pause(ticks);
    fprintf(1, "(nothing happends for a little while)\n");

    exit(0);
}