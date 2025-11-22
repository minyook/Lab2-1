#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <systemd/sd-bus.h>

// DBus 서비스 정보 정의 (PDF 99페이지 참고)
#define SERVICE_NAME "org.example.NotificationService"
#define OBJECT_PATH  "/org/example/NotificationService"
#define INTERFACE_NAME "org.example.NotificationService"

static int notification_count = 0;

/**
 * @brief 클라이언트의 메서드 호출에 응답하는 핸들러 (선택적)
 * 이 예제에서는 클라이언트가 서버에 "Hello"를 호출하여 초기 연결 테스트를 할 수 있습니다.
 */
static int handle_hello(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
    const char *client_name;
    char reply_msg[64];
    
    // 클라이언트로부터 인자 읽기 ("s" = string)
    int ret = sd_bus_message_read(m, "s", &client_name);
    if (ret < 0) return ret;
    
    notification_count++;
    sprintf(reply_msg, "Hello %s! Server has processed %d notifications.", 
            client_name, notification_count);
            
    // 클라이언트에게 메서드 결과 회신 ("s" = string)
    return sd_bus_reply_method_return(m, "s", reply_msg);
}

// DBus 인터페이스 정의 테이블 (VTable)
// 메서드(handle_hello)와 시그널(Notification)을 등록합니다. (PDF 98페이지 참고)
static const sd_bus_vtable service_vtable[] = {
    SD_BUS_VTABLE_START(0),
    SD_BUS_METHOD("Hello", "s", "s", handle_hello, SD_BUS_VTABLE_UNPRIVILEGED),
    SD_BUS_SIGNAL("Notification", "s", 0), // "s": 시그널 인자 타입 (문자열)
    SD_BUS_VTABLE_END
};

int main() {
    sd_bus_slot *slot = NULL;
    sd_bus *bus = NULL;
    int ret = 0;

    // 1. Session Bus에 연결 (PDF 99페이지 참고)
    ret = sd_bus_default_user(&bus);
    if (ret < 0) {
        fprintf(stderr, "Failed to connect to D-Bus daemon: %s\n", strerror(-ret));
        goto finish;
    }

    // 2. 객체 경로 및 인터페이스 등록 (PDF 99페이지 참고)
    ret = sd_bus_add_object_vtable(bus, &slot, OBJECT_PATH, INTERFACE_NAME, service_vtable, NULL);
    if (ret < 0) {
        fprintf(stderr, "Failed to add object interface: %s\n", strerror(-ret));
        goto finish;
    }

    // 3. Bus Name 요청 (클라이언트가 이 이름으로 서버를 찾음) (PDF 99페이지 참고)
    ret = sd_bus_request_name(bus, SERVICE_NAME, 0);
    if (ret < 0) {
        fprintf(stderr, "Failed to acquire service name: %s\n", strerror(-ret));
        goto finish;
    }

    printf("D-Bus Server running as %s. Sending notifications...\n", SERVICE_NAME);
    
    // 4. 메인 루프: 주기적으로 시그널 방출 및 버스 메시지 처리
    while (1) {
        // 버스 메시지 처리 (클라이언트의 메서드 호출 등)
        ret = sd_bus_process(bus, NULL);
        if (ret < 0) break;

        // 메시지 처리가 없으면 잠시 대기
        if (ret == 0) sd_bus_wait(bus, (uint64_t)(1000 * 1000)); // 1s 대기

        // 5. 시그널 방출 (알림 메시지)
        char message[64];
        sprintf(message, "Time is %ld. New message!", (long)time(NULL));
        
        // sd_bus_emit_signal을 사용하여 Notification 시그널을 보냅니다.
        ret = sd_bus_emit_signal(bus, OBJECT_PATH, INTERFACE_NAME, "Notification", "s", message);
        if (ret < 0) {
            fprintf(stderr, "Failed to emit signal: %s\n", strerror(-ret));
        } else {
            printf("[Server] Emitted Notification: %s\n", message);
        }
    }

finish:
    sd_bus_slot_unref(slot);
    sd_bus_unref(bus);
    return ret < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
