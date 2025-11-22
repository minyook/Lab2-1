#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

int main() {
    pid_t pid;
    
    printf("--- fork() 호출 전 ---\n");
    printf("나의 PID: %d\n", getpid());
    
    pid = fork(); // 새로운 프로세스 생성
    
    if (pid < 0) {
        perror("fork failed");
        return 1;
    } else if (pid == 0) {
        // 자식 프로세스 코드 (pid = 0)
        printf("\n--- 자식 프로세스 ---\n");
        printf("PID: %d, 부모 PID: %d\n", getpid(), getppid());
    } else {
        // 부모 프로세스 코드 (pid > 0, 자식의 PID를 가짐)
        printf("\n--- 부모 프로세스 ---\n");
        printf("PID: %d, 자식 PID: %d\n", getpid(), pid);
    }
    
    return 0;
}
