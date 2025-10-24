#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "kernel/param.h"

char** getncmd(int argc, char *argv[], char *filename) {
    static char param[MAXARG][MAXPATH];
    static char *ptrs[MAXARG];

    int n = argc - 4;
    int i = 0;
    for (; i < n; i++) {
        strcpy(param[i], argv[i+4]);
        ptrs[i] = param[i];
    }
    strcpy(param[i], filename);
    ptrs[i] = param[i];
    ptrs[++i] = 0;
    return ptrs;
}

void find(int argc, char *argv[], char *path, char *filename)
{
    int fd;
    char buf[512], *p;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, O_RDONLY)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch (st.type)
    {
    case T_DEVICE:
    case T_FILE:
        fprintf(2, "find: %s is not a valid path\n", path);
        break;
    case T_DIR:
        if (strlen(path)+1+DIRSIZ+1 > sizeof buf) {
            fprintf(2, "find: path too long\n");
            break;
        }
        strcpy(buf, path);
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
                if (strcmp(de.name, filename) == 0) {
                    if (argc == 3)
                        printf("%s\n", buf);
                    else {
                        if (fork() == 0) {
                            exec(argv[4], getncmd(argc, argv, buf));
                            fprintf(2, "exec: %s failed\n", argv[4]);
                            exit(1);
                        }
                        wait(0);
                    }
                }
                break;
            case T_DIR:
                find(argc, argv, buf, filename);
                break;
            }
        }
    }
    close(fd);
}

int main(int argc, char *argv[])
{
    if (!(argc == 3 || (argc>=5 && strcmp(argv[3], "-exec")==0))) {
        fprintf(2, "Uasge: find <path> <filename> [-exec cmd]\n");
        exit(1);
    }

    find(argc, argv, argv[1], argv[2]);
    exit(0);
}