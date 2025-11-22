#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h> // wait 함수와 매크로(WIFEXITED, WEXITSTATUS)에 필요

int main() {
    pid_t pid;
    int status;
    
    pid = fork();
    
    if (pid < 0) {
        perror("fork failed");
        return 1;
    } else if (pid == 0) {
        // 자식 프로세스 (5초 동안 작업 후 종료 상태 10 반환)
        printf("자식 프로세스 시작. 5초 동안 작업...\n");
        sleep(5);
        printf("자식 프로세스 종료 (Exit Status: 10)\n");
        exit(10); // exit() 시스템 호출로 종료 상태 반환
    } else {
        // 부모 프로세스: 자식이 끝날 때까지 대기
        printf("부모 프로세스: 자식 %d가 종료될 때까지 대기합니다.\n", pid);
        
        // wait() 호출: 자식이 종료될 때까지 블록됨. 자식의 상태 정보는 status에 저장됨.
        if (wait(&status) == -1) {
            perror("wait failed");
            exit(2);
        }

        printf("부모 프로세스: 자식 %d 종료 확인.\n", pid);
        
        // 종료 상태 확인 매크로 (PDF 19페이지 참고)
        if (WIFEXITED(status)) {
            printf("자식 프로세스의 종료 상태: %d\n", WEXITSTATUS(status));
        }
    }
    return 0;
}
