#include <stdio.h>
 #include <unistd.h>
#include <stdlib.h>
 #define BUFSIZE 256
 int main()
 {
    int n;
    char buf[BUFSIZE];
    fprintf(stderr, "Here is file start.\n");
    while ((n = read(0, buf, BUFSIZE)) > 0)
        write(1, buf, n);
    fprintf(stderr, "Here is file end.\n");
    exit(0);
 }
