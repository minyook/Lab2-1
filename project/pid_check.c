#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    pid_t my_pid = getpid();
    pid_t parent_pid = getppid();
    
    printf("---------------------------\n");
    printf("현재 프로세스 ID (PID): %d\n", my_pid);
    printf("부모 프로세스 ID (PPID): %d\n", parent_pid);
    printf("---------------------------\n");
    
    return 0;
}
