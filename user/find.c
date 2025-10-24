#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

void find(char *dirname, char *filename)
{
    int fd;
    char buf[512], *p;
    struct dirent de;
    struct stat st;

    if ((fd = open(dirname, O_RDONLY)) < 0) {
        fprintf(2, "find: cannot open %s\n", dirname);
        return;
    }

    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", dirname);
        close(fd);
        return;
    }

    switch (st.type)
    {
    case T_DEVICE:
    case T_FILE:
        fprintf(2, "find: %s is not a valid path\n", dirname);
        break;
    case T_DIR:
        if (strlen(dirname)+1+DIRSIZ+1 > sizeof buf) {
            fprintf(2, "find: path too long\n");
            break;
        }
        strcpy(buf, dirname);
        p = buf + strlen(buf);
        *p++ = '/';
        while (read(fd, &de, sizeof(de)) == sizeof(de)) {
            if (de.inum == 0)
                continue;
            if (strcmp(de.name, ".")==0 || strcmp(de.name, "..")==0)
                continue;
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            if (stat(buf, &st) < 0) {
                fprintf(2, "find: cannot stat %s\n", buf);
                continue;
            }
            switch (st.type) {
            case T_DEVICE:
            case T_FILE:
                if (strcmp(de.name, filename) == 0)
                    printf("%s\n", buf);
                break;
            case T_DIR:
                find(buf, filename);
                break;
            }
        }
    }
    close(fd);
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(2, "Uasge: find <dirname> <filename>\n");
        exit(1);
    }

    find(argv[1], argv[2]);
    exit(0);
}