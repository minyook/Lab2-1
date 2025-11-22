#include <stdio.h>
#include <signal.h> // sigaction 함수와 구조체에 필요
#include <unistd.h> // sleep 함수에 필요
#include <string.h>

// 시그널 핸들러 함수
void sigint_handler(int signo) {
    printf("\n[Handler] SIGINT (Signal %d)를 받았습니다. 프로그램을 종료하지 않습니다.\n", signo);
}

int main() {
    struct sigaction act;
    
    // sigaction 구조체 설정 (PDF 48페이지 참고)
    memset(&act, 0, sizeof(act));
    act.sa_handler = sigint_handler; // SIGINT 수신 시 호출될 함수 지정
    
    // 시그널 핸들러를 설정하는 동안 모든 시그널을 봉쇄 (sa_mask)
    sigfillset(&(act.sa_mask)); 
    
    // SIGINT(인터럽트, 보통 Ctrl-C) 시그널에 대해 설정 적용
    // (PDF 49페이지 handlesignal.c 참고)
    if (sigaction(SIGINT, &act, NULL) == -1) {
        perror("sigaction failed");
        return 1;
    }
    
    printf("SIGINT 핸들러 설정 완료. Ctrl-C를 눌러보세요. (5초마다 메시지 출력)\n");
    
    while(1) {
        sleep(5);
        printf("작업 중...\n");
    }
    
    return 0; // 이 코드는 Ctrl-C를 눌러도 실행되지 않음
}
