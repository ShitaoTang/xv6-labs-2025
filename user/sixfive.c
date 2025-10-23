#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

#define MAX_BYTES 4096

const char separators[7] = {'-', '\r', '\t', '\n', '.', '/', ','};

int closest_sep(const char *str)
{
    int min = MAX_BYTES;
    char *pos;
    for (int i = 0; i < 7; i++) {
        pos = strchr(str, separators[i]); 
        if (pos && pos-str < min)
            min = pos-str;
    }

    if (min == MAX_BYTES) return -1;
    return min;
}

int num(const char *str, int left, int right)
{
    if (left > right) return -1;
    
    int res = 0;
    while (left < right) {
        res = res*10 + str[left]-'0';
        left++;
    }
    res = res*10 + str[right]-'0';

    return res;
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(2, "Usage: sixfive <filename> [<filename> ...] \n");
        exit(1);
    }

    char buf[MAX_BYTES];
    int nbytes = 0;
    int n = 0;

    for (int i = 1; i < argc; i++) {
        int fd = open(argv[i], O_RDONLY);
        if (fd < 0) {
            fprintf(2, "Open file failed!\n");
            exit(1);
        }
        while((n = read(fd, buf+nbytes, MAX_BYTES)) > 0)
            nbytes += n;
        if (n < 0) {
            fprintf(2, "Read file failed!\n");
            close(fd);
            exit(1);
        }
        close(fd);
    }
    if (!nbytes) exit(0);
    buf[nbytes] = '\0';

    int left = -1, right = 0, number;

    while ((right = closest_sep(buf)) != -1) {
        buf[right] = 'x';
        number = num(buf, left+1, right-1);
        if (number%5==0 || number%6==0) printf("%d\n", number);
        left = right;
    }

    if (right == -1) right = nbytes;
    number = num(buf, left+1, right-1);
    if (number%5==0 || number%6==0) printf("%d\n", number);

    exit(0);
}