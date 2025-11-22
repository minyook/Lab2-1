#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
void child_handler(int signo) {
    printf("[자식] SIGUSR1 시그널 수신, 종료합니다.\n");
    exit(0);
}

int main() {
    pid_t pid;
    
    if ((pid = fork()) < 0) {
        perror("fork failed");
        return 1;
    } 
    
    if (pid == 0) {
        // 자식 프로세스: SIGUSR1 핸들러 설정 후 대기
        signal(SIGUSR1, child_handler); // 간단한 시그널 설정 함수 사용
        printf("[자식] PID %d: 부모의 시그널을 기다리는 중...\n", getpid());
        pause(); // 시그널이 도착할 때까지 실행을 중단하고 대기 (PDF 54페이지)
    } else {
        // 부모 프로세스: 자식 프로세스가 준비될 시간을 준 후 시그널 전송
        printf("[부모] PID %d: 2초 후 자식에게 시그널 전송.\n", getpid());
        sleep(2);
        
        // kill 시스템 호출로 자식 프로세스(pid)에게 SIGUSR1 전송 (PDF 53페이지)
        if (kill(pid, SIGUSR1) == -1) {
            perror("kill failed");
        }
        
        wait(NULL); // 자식 종료 대기
        printf("[부모] 자식 종료 확인.\n");
    }
    return 0;
}
