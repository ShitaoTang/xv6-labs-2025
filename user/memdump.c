#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

void memdump(char *fmt, char *data);

int
main(int argc, char *argv[])
{
  if(argc == 1){
    printf("Example 1:\n");
    int a[2] = { 61810, 2025 };
    memdump("ii", (char*) a);
    
    printf("Example 2:\n");
    memdump("S", "a string");
    
    printf("Example 3:\n");
    char *s = "another";
    memdump("s", (char *) &s);

    struct sss {
      char *ptr;
      int num1;
      short num2;
      char byte;
      char bytes[8];
    } example;
    
    example.ptr = "hello";
    example.num1 = 1819438967;
    example.num2 = 100;
    example.byte = 'z';
    strcpy(example.bytes, "xyzzy");
    
    printf("Example 4:\n");
    memdump("pihcS", (char*) &example);
    
    printf("Example 5:\n");
    memdump("sccccc", (char*) &example);
  } else if(argc == 2){
    // format in argv[1], up to 512 bytes of data from standard input.
    char data[512];
    int n = 0;
    memset(data, '\0', sizeof(data));
    while(n < sizeof(data)){
      int nn = read(0, data + n, sizeof(data) - n);
      if(nn <= 0)
        break;
      n += nn;
    }
    memdump(argv[1], data);
  } else {
    printf("Usage: memdump [format]\n");
    exit(1);
  }
  exit(0);
}

void
memdump(char *fmt, char *data)
{
    while (*fmt) {
        switch (*fmt) {
        case 'i':
            int inum = 0;
            for (int i=0, factor=1; i<4; i++, data++, factor*=16*16)
                inum += *data * factor;
            printf("%d\n", inum);
            break;
        case 'p':
            long lnum = 0;
            for (int i=0, factor=1; i<8; i++, data++, factor*=16*16)
                lnum += *data * factor;
            printf("%lx\n", lnum);
            break;
        case 'h':
            short snum = 0;
            for (int i=0, factor=1; i<2; i++, data++, factor*=16*16)
                snum += *data * factor;
            printf("%d\n", snum);
            break;
        case 'c':
            printf("%c\n", *data++);
            break;
        case 's':
            char *str = 0;
            for (int i=0, factor=1; i<8; i++, data++, factor*=16*16)
                str += *data * factor;
            printf("%s\n", str);
            break;
        case 'S':
            printf("%s\n", data);
            break;
        default:
            fprintf(2, "Invalid type '%c', must be one of: [iphcsS]", *fmt);
            exit(1);
        }
        fmt++;
    }
}
