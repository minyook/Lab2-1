#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sysv_msg_defs.h"

int main() {
    int qid;
    struct msgq_data send_data = {1, "System V Message"}; 

    // 1. 메시지 큐 생성/개방
    if ((qid = msgget(QKEY, IPC_CREAT | 0666)) == -1) {
        perror("msgget failed");
        exit(1);
    }
    printf("Sender: Message Queue (ID: %d) 개방 성공.\n", qid);

    // 2. 메시지 전송
    if (msgsnd(qid, &send_data, strlen(send_data.text) + 1, 0) == -1) {
        perror("msgsnd failed");
        exit(1);
    }
    printf("Sender: Message '%s' 전송 완료.\n", send_data.text);

    return 0;
}

