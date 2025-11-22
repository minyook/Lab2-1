#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sysv_msg_defs.h"

int main() {
    int qid;
    struct msgq_data recv_data;
    
    // 1. 메시지 큐 개방
    if ((qid = msgget(QKEY, 0)) == -1) { 
        perror("msgget failed");
        exit(1);
    }

    // 2. 메시지 수신
    if (msgrcv(qid, &recv_data, BUFSIZE, 0, 0) == -1) {
        perror("msgrcv failed");
        exit(1);
    }
    printf("Receiver: Received message: %s\n", recv_data.text);

    // 3. 메시지 큐 삭제
    if (msgctl(qid, IPC_RMID, NULL) == -1) { 
        perror("msgctl IPC_RMID failed");
        exit(1);
    }
    printf("Receiver: Message Queue (ID: %d) 삭제 완료.\n", qid);
    
    return 0;
}
