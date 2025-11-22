#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <errno.h>

#define MAX_MSG_SIZE 256
#define QKEY (key_t)0x1000 // 고유한 키 (PDF 50페이지 참고)

// 메시지 큐 구조체 (PDF 47페이지 참고)
struct msgq_data {
    long mtype;             // 메시지 유형 (수신자 식별에 사용)
    char mtext[MAX_MSG_SIZE]; // 메시지 데이터
};

// 함수 원형
void chat_loop(int qid, long send_type, long recv_type, const char *role);

int main() {
    int qid;
    pid_t pid;

    // 1. 메시지 큐 생성/개방
    // IPC_CREAT | 0666: 없으면 생성하고, 권한은 읽기/쓰기 허용 (PDF 46페이지 참고)
    if ((qid = msgget(QKEY, IPC_CREAT | 0666)) == -1) {
        perror("msgget failed");
        exit(1);
    }
    printf("Message Queue (ID: %d) 개방 성공.\n", qid);

    // 2. 프로세스 생성
    pid = fork();

    if (pid < 0) {
        perror("fork failed");
        // 오류 시 큐 삭제
        msgctl(qid, IPC_RMID, NULL);
        exit(1);
    } 
    
    if (pid == 0) {
        /* --- 자식 프로세스 --- */
        // 자식은 유형 2로 보내고 (송신), 유형 1을 받습니다 (수신).
        chat_loop(qid, 2, 1, "Child");
    } else {
        /* --- 부모 프로세스 --- */
        // 부모는 유형 1로 보내고 (송신), 유형 2를 받습니다 (수신).
        printf("Parent Process (PID: %d) started chat. Type 'quit' to exit.\n", getpid());
        chat_loop(qid, 1, 2, "Parent");

        wait(NULL); // 자식 종료 대기
        
        // 3. 큐 삭제 (프로그램 종료 시)
        if (msgctl(qid, IPC_RMID, NULL) == -1) {
            perror("msgctl IPC_RMID failed");
            exit(1);
        }
        printf("\nMessage Queue 삭제 완료.\n");
    }
    
    return 0;
}

/**
 * @brief 채팅 루프: 메시지 큐로 메시지를 송신하고, 특정 유형의 메시지를 수신합니다.
 * @param qid 메시지 큐 식별자
 * @param send_type 송신 메시지 유형
 * @param recv_type 수신할 메시지 유형
 * @param role 현재 프로세스의 역할
 */
void chat_loop(int qid, long send_type, long recv_type, const char *role) {
    struct msgq_data s_data, r_data;
    char input_buf[MAX_MSG_SIZE];

    // 메시지 유형 설정
    s_data.mtype = send_type;

    while (1) {
        // --- 1. 키보드 입력 및 송신 ---
        printf("[%s] Enter message: ", role);
        if (fgets(input_buf, MAX_MSG_SIZE, stdin) == NULL) continue;
        input_buf[strcspn(input_buf, "\n")] = 0; // 개행 문자 제거

        // 송신 데이터 준비
        strncpy(s_data.mtext, input_buf, MAX_MSG_SIZE);
        
        // msgsnd: 큐에 메시지 전송 (PDF 47페이지 참고)
        if (msgsnd(qid, &s_data, strlen(s_data.mtext) + 1, 0) == -1) {
            perror("msgsnd failed");
            break;
        }

        // 'quit' 명령 처리
        if (strcmp(input_buf, "quit") == 0) {
            printf("[%s] Sent 'quit', exiting.\n", role);
            break;
        }

        // --- 2. 메시지 수신 대기 및 처리 ---
        // msgrcv: 특정 유형(recv_type)의 메시지를 수신 (PDF 48페이지 참고)
        // IPC_NOWAIT 플래그가 없으므로 메시지가 도착할 때까지 대기합니다.
        // 다만, 여기서는 송신 직후 수신 대기를 시작하여 동기화합니다.
        
        printf("[%s] Waiting for partner...\n", role);
        if (msgrcv(qid, &r_data, MAX_MSG_SIZE, recv_type, 0) == -1) {
            // EAGAIN은 nonblocking 모드에서만 발생하므로, 여기서는 보통 오류로 간주
            perror("msgrcv failed");
            break;
        }
        
        printf("[%s received]: %s\n", role == "Parent" ? "Child" : "Parent", r_data.mtext);

        // 상대방의 'quit' 명령 처리
        if (strcmp(r_data.mtext, "quit") == 0) {
            printf("[%s] Partner quit, exiting.\n", role);
            break;
        }
    }
}
