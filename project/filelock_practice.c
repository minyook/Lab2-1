#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h> 
#include <sys/wait.h> 

int main() {
    int fd;
    struct flock testlock;
    pid_t pid;

    // 1. 파일 열기 및 생성
    fd = open("testlock", O_RDWR | O_CREAT, 0666);
    if (fd == -1) { 
        perror("open failed"); 
        exit(1); 
    }

    // 2. 부모 프로세스의 쓰기 잠금 인자 지정
    testlock.l_type = F_WRLCK;      
    testlock.l_whence = SEEK_SET;   
    testlock.l_start = 0;           
    testlock.l_len = 10;            
    
    // 3. 잠금 시도: F_SETLKW (잠금 해제를 기다림)
    if (fcntl(fd, F_SETLKW, &testlock) == -1) { 
        perror("parent: locking failed"); 
        exit(1); 
    }
    printf("parent: locked record (0~9 bytes)\n");

    // 4. 자식 프로세스 생성
    pid = fork();
    if (pid < 0) { 
        perror("fork failed"); 
        exit(1); 
    }
    
    if (pid == 0) { /* --- 자식 프로세스 --- */
        testlock.l_len = 5; 
        
        // 부모가 잠금을 해제할 때까지 여기서 대기 (F_SETLKW)
        printf("child: waiting for lock...\n");
        if (fcntl(fd, F_SETLKW, &testlock) == -1) { 
            perror("child: locking failed"); 
            exit(1); 
        }
        printf("child: locked successfully\n");
        sleep(2);
        printf("child: exiting\n");
        exit(0);
    } else { /* --- 부모 프로세스 --- */
        sleep(5); 
        // 부모가 종료되면 잠금은 자동으로 해제됩니다.
        printf("parent: exiting (lock released)\n");
        wait(NULL); 
    }
    
    close(fd);
    return 0;
}
