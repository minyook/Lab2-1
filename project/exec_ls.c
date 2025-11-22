#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    char *const argv[] = {"ls", "-l", NULL};
    
    printf("--- execvp()를 사용하여 'ls -l' 명령을 실행합니다 ---\n");
    
    // 현재 프로세스의 메모리 공간을 /bin/ls 프로그램으로 대체
    execvp("ls", argv);
    
    // execvp가 성공하면 이 코드는 실행되지 않습니다.
    perror("execvp failed to run ls"); 
    exit(1);
}
