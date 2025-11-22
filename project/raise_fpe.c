#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

int main() {
    int a = 10;
    int b = 0;
    
    printf("Zero Division Error를 인위적으로 발생시킵니다.\n");
    
    if (b == 0) {
        // b가 0일 경우, 부동 소수점 예외 시그널(SIGFPE)을 자신에게 전송 (PDF 58페이지)
        raise(SIGFPE); 
    }
    
    // raise() 호출 후, 시그널이 처리되므로 이 코드는 실행되지 않음
    a = a / b;
    
    printf("이 메시지는 출력되지 않습니다.\n");
    return 0;
}
