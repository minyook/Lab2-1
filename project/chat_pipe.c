#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MSGSIZE 256

void chat_loop(int read_fd, int write_fd, const char *role, const char *partner);

int main() {
    int p1[2], p2[2]; // p1: 부모 -> 자식, p2: 자식 -> 부모
    pid_t pid;

    // 1. 첫 번째 파이프 생성 (p1: 부모 -> 자식)
    if (pipe(p1) == -1) {
        perror("pipe p1 failed");
        exit(1);
    }
    
    // 2. 두 번째 파이프 생성 (p2: 자식 -> 부모)
    if (pipe(p2) == -1) {
        perror("pipe p2 failed");
        exit(1);
    }

    // 3. 프로세스 생성
    pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(1);
    } 
    
    if (pid == 0) {
        /* --- 자식 프로세스 (Child) --- */
        
        // p1: 읽기 채널 (부모로부터 받음)
        close(p1[1]); // p1의 쓰기용 기술자 닫기
        // p2: 쓰기 채널 (부모에게 보냄)
        close(p2[0]); // p2의 읽기용 기술자 닫기

        printf("Child Process (PID: %d) started chat.\n", getpid());
        chat_loop(p1[0], p2[1], "Child", "Parent");

        close(p1[0]);
        close(p2[1]);
        exit(0);

    } else {
        /* --- 부모 프로세스 (Parent) --- */
        
        // p1: 쓰기 채널 (자식에게 보냄)
        close(p1[0]); // p1의 읽기용 기술자 닫기
        // p2: 읽기 채널 (자식으로부터 받음)
        close(p2[1]); // p2의 쓰기용 기술자 닫기

        printf("Parent Process (PID: %d) started chat. Type 'quit' to exit.\n", getpid());
        chat_loop(p2[0], p1[1], "Parent", "Child");

        close(p1[1]);
        close(p2[0]);
        wait(NULL); // 자식 종료 대기
    }
    
    return 0;
}

/**
 * @brief 채팅 루프: 메시지를 읽고 쓰는 작업을 반복합니다.
 * @param read_fd 메시지 수신용 파일 기술자
 * @param write_fd 메시지 송신용 파일 기술자
 * @param role 현재 프로세스의 역할 ("Parent" 또는 "Child")
 * @param partner 상대방 프로세스의 역할
 */
void chat_loop(int read_fd, int write_fd, const char *role, const char *partner) {
    char send_buf[MSGSIZE];
    char recv_buf[MSGSIZE];
    fd_set read_fds; // select 함수를 위한 파일 기술자 집합 [cite: 661]
    int max_fd = (read_fd > STDIN_FILENO) ? read_fd : STDIN_FILENO;

    while (1) {
        FD_ZERO(&read_fds);         // 집합 초기화 [cite: 665]
        FD_SET(STDIN_FILENO, &read_fds); // 표준 입력(키보드) 추가 
        FD_SET(read_fd, &read_fds);     // 파이프 읽기 채널 추가 

        // 파일 기술자 집합 중 읽기 준비된 것이 있는지 검사 [cite: 658, 661]
        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select error");
            break;
        }

        // 1. 파이프에서 데이터 수신
        if (FD_ISSET(read_fd, &read_fds)) { // 파이프에 데이터가 있으면 
            ssize_t nbytes = read(read_fd, recv_buf, MSGSIZE);
            if (nbytes > 0) {
                recv_buf[nbytes] = '\0';
                printf("\n[%s received]: %s\n", partner, recv_buf);
                if (strcmp(recv_buf, "quit") == 0) {
                    printf("[%s] partner quit. Exiting.\n", role);
                    break;
                }
            } else if (nbytes == 0) {
                // 파이프가 닫혔을 때 (상대방이 종료)
                printf("\n[%s] partner pipe closed. Exiting.\n", role);
                break;
            } else {
                perror("read pipe error");
                break;
            }
        }
        
        // 2. 키보드 입력 처리
        if (FD_ISSET(STDIN_FILENO, &read_fds)) { // 키보드 입력이 있으면 
            if (fgets(send_buf, MSGSIZE, stdin) == NULL) continue;
            
            // 개행 문자 제거
            send_buf[strcspn(send_buf, "\n")] = 0; 
            
            if (strlen(send_buf) > 0) {
                // 파이프에 메시지 전송
                write(write_fd, send_buf, strlen(send_buf) + 1);
                
                if (strcmp(send_buf, "quit") == 0) {
                    printf("[%s] Sent 'quit'. Exiting.\n", role);
                    break;
                }
                
                printf("[%s sent]: %s\n", role, send_buf);
            }
        }
    }
}
