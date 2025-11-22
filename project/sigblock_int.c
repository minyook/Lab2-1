#include <stdio.h>
#include <signal.h> // sigprocmask, sigset_t, sigemptyset, sigaddset에 필요
#include <unistd.h> // sleep 함수에 필요

int main() {
    sigset_t set_int;
    
    // 1. 시그널 집합 초기화 및 SIGINT(Ctrl-C) 추가 (PDF 62페이지 blocksignal.c 참고)
    sigemptyset(&set_int); // 비어있는 시그널 집합 생성
    sigaddset(&set_int, SIGINT); // SIGINT(Ctrl-C)를 집합에 추가
    
    printf("--- 임계 영역 시작: SIGINT 차단 (5초간) ---\n");
    
    // 2. SIG_BLOCK: 현재 시그널 마스크에 set_int에 있는 시그널들을 추가 (차단)
    sigprocmask(SIG_BLOCK, &set_int, NULL);
    
    printf("Ctrl-C를 눌러도 무시됩니다. (5초 동안)\n");
    sleep(5);
    
    printf("--- 임계 영역 종료: SIGINT 차단 해제 ---\n");
    
    // 3. SIG_UNBLOCK: set_int에 있는 시그널들을 차단 해제
    sigprocmask(SIG_UNBLOCK, &set_int, NULL);
    
    printf("이제 Ctrl-C가 작동합니다. (5초 동안 대기)\n");
    sleep(5); 
    
    return 0;
}

