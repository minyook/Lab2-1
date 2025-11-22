#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#define MSGSIZE 64
#define FIFO_NAME "my_fifo_pipe"

int main() {
    int fd;
    char buf[MSGSIZE];

    // 1. FIFO를 읽기/쓰기 전용으로 열기
    if ((fd = open(FIFO_NAME, O_RDWR)) < 0) { 
        perror("fifo open failed (reader)");
        exit(1);
    }
    printf("Reader: FIFO 열기 성공. 메시지 수신 대기 중...\n");

    // 2. 메시지 수신
    if (read(fd, buf, MSGSIZE) < 0) {
        perror("fifo read failed");
        exit(1);
    }
    printf("Reader: Received message: %s\n", buf);
    
    close(fd);
    
    // 3. 정리: FIFO 파일 삭제
    if (unlink(FIFO_NAME) == -1) { 
        perror("unlink failed");
    }
    
    return 0;
}
