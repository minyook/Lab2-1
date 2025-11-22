#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

void alarm_handler(int sig) {
    printf("\n[Handler] SIGALRM 수신: 5초 타이머 종료.\n");
    exit(0); // 프로그램 종료
}

int main() {
    // SIGALRM 시그널 핸들러 설정
    signal(SIGALRM, alarm_handler);
    
    printf("5초 타이머를 설정합니다.\n");
    
    // 5초 후에 SIGALRM 시그널을 자신에게 전송하도록 예약
    alarm(5); 
    
    printf("타이머가 작동하는 동안 무한 루프를 실행합니다. (5초 후 종료)\n");
    
    // 프로그램이 즉시 끝나는 것을 방지하기 위해 무한 루프를 돌거나 pause()를 사용
    while (1) {
        // CPU 사용을 줄이기 위해 잠깐 쉼
        sleep(1);
    }
    
    return 0;
}
