#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // pipe(), fork(), read(), write()
#include <string.h>
#include <sys/wait.h> // wait() 사용

#define MSGSIZE 64

int main() {
    char buf[MSGSIZE];
    int p[2]; // p[0]: 읽기용, p[1]: 쓰기용 파일 기술자 쌍
    pid_t pid;
    const char *msg1 = "Hello from Child #1";
    const char *msg2 = "Hello from Child #2";

    // 1. 파이프 생성
    if (pipe(p) == -1) {
        perror("pipe call failed");
        exit(1);
    }

    // 2. 프로세스 생성
    pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(1);
    } else if (pid == 0) { /* --- 자식 프로세스 (쓰기 담당) --- */
        close(p[0]); // 자식은 읽기용 기술자 p[0]을 닫음

        // 3. 파이프에 메시지 쓰기
        write(p[1], msg1, strlen(msg1) + 1);
        write(p[1], msg2, strlen(msg2) + 1);
        
        close(p[1]); // 쓰기 완료 후 쓰기용 기술자 닫기
        exit(0);
    } else { /* --- 부모 프로세스 (읽기 담당) --- */
        close(p[1]); // 부모는 쓰기용 기술자 p[1]을 닫음

        // 4. 파이프로부터 메시지 읽기
        printf("Parent: Reading messages from pipe...\n");
        if (read(p[0], buf, MSGSIZE) > 0) { // 메시지 1 읽기
            printf("Received: %s\n", buf);
        }
        if (read(p[0], buf, MSGSIZE) > 0) { // 메시지 2 읽기
            printf("Received: %s\n", buf);
        }

        close(p[0]); // 읽기 완료 후 읽기용 기술자 닫기
        wait(NULL); // 자식 종료 대기
    }
    return 0;
}
