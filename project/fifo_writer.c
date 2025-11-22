#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h> // EEXIST 사용

#define MSGSIZE 64
#define FIFO_NAME "my_fifo_pipe"

int main(int argc, char **argv) {
    int fd;
    char buf[MSGSIZE];
    
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <message>\n", argv[0]);
        exit(1);
    }

    // 1. FIFO 파일 생성 (이미 존재하면 오류 무시)
    if (mkfifo(FIFO_NAME, 0666) == -1 && errno != EEXIST) {
        perror("mkfifo failed");
        exit(1);
    }
    printf("Writer: FIFO '%s' 생성 완료.\n", FIFO_NAME);

    // 2. FIFO를 쓰기 전용으로 열기 
    if ((fd = open(FIFO_NAME, O_WRONLY)) < 0) {
        perror("fifo open failed (writer)");
        exit(1);
    }
    printf("Writer: FIFO 열기 성공. 메시지 전송 시작.\n");

    // 3. 메시지 전송
    strncpy(buf, argv[1], MSGSIZE - 1);
    buf[MSGSIZE - 1] = '\0'; // 안전하게 문자열 종료
    
    if (write(fd, buf, MSGSIZE) < 0) {
        perror("fifo write failed");
        exit(1);
    }
    printf("Writer: Sent message: '%s'\n", buf);
    
    close(fd);
    return 0;
}
