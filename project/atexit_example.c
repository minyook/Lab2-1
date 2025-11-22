#include <stdio.h>
#include <stdlib.h> // atexit 함수에 필요

void cleanup_file(void) {
    printf(">> [Exit Routine 2] 임시 파일 정리 완료.\n");
}

void log_termination(void) {
    printf(">> [Exit Routine 1] 프로그램 종료 로그 기록.\n");
}

int main() {
    // 종료 루틴 등록 (역순으로 실행됨)
    atexit(cleanup_file); 
    atexit(log_termination);
    
    printf("메인 함수 시작.\n");
    printf("메인 함수 로직 수행 중...\n");
    
    // main 함수가 끝날 때 (exit(0)이 명시되거나 main이 리턴될 때) 등록된 함수들이 호출됨
    return 0; 
}

