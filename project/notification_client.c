#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <systemd/sd-bus.h>

// DBus 서비스 정보 정의 (서버와 동일)
#define SERVICE_NAME "org.example.NotificationService"
#define OBJECT_PATH  "/org/example/NotificationService"
#define INTERFACE_NAME "org.example.NotificationService"

/**
 * @brief 서버로부터 "Notification" 시그널을 수신했을 때 호출되는 핸들러
 */
static int on_notification(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
    const char *notification_text;
    
    // 시그널 인자 읽기 ("s" = string)
    int ret = sd_bus_message_read(m, "s", &notification_text);
    if (ret < 0) return ret;
    
    printf("\n[Client] *** RECEIVED NOTIFICATION ***\n");
    printf("[Client] Message: %s\n", notification_text);
    printf("[Client] *******************************\n");
    
    return 0;
}

int main() {
    sd_bus_error error = SD_BUS_ERROR_NULL;
    sd_bus_message *m = NULL;
    sd_bus *bus = NULL;
    sd_bus_slot *sigslot = NULL; // 시그널 매치 슬롯
    const char *reply_msg;
    int ret;

    // 1. Session Bus에 연결
    ret = sd_bus_default_user(&bus);
    if (ret < 0) {
        fprintf(stderr, "Failed to connect to D-Bus daemon: %s\n", strerror(-ret));
        goto finish;
    }
    
    // 2. 서버의 "Hello" 메서드 호출 (연결 및 응답 테스트) (PDF 100페이지 참고)
    ret = sd_bus_call_method(bus,
                             SERVICE_NAME, OBJECT_PATH, INTERFACE_NAME, "Hello",
                             &error, &m, "s", "DbusClient");
    if (ret < 0) {
        fprintf(stderr, "Failed to call Hello method: %s\n", error.message);
        goto finish;
    }
    
    // 응답 메시지 읽기
    sd_bus_message_read(m, "s", &reply_msg);
    printf("[Client] Server response (Method Call): %s\n", reply_msg);
    
    // 3. 시그널 수신 매치 룰 추가 (PDF 101페이지 참고)
    // type='signal', interface=INTERFACE_NAME, member='Notification' 시그널에 매치
    ret = sd_bus_add_match(bus, &sigslot,
                           "type='signal',interface='" INTERFACE_NAME "',member='Notification'",
                           on_notification, NULL);
    if (ret < 0) {
        fprintf(stderr, "Failed to add signal match: %s\n", strerror(-ret));
        goto finish;
    }

    printf("[Client] Listening for Notifications from %s...\n", SERVICE_NAME);

    // 4. 메인 루프: 버스로 들어오는 메시지(시그널 포함)를 처리
    while (1) {
        // 버스 메시지를 처리
        ret = sd_bus_process(bus, NULL);
        if (ret < 0) break;

        // 처리할 메시지가 없으면 대기 (non-blocking)
        if (ret == 0) sd_bus_wait(bus, (uint64_t)(1000 * 1000)); // 1s 대기
    }
    
finish:
    sd_bus_error_free(&error);
    sd_bus_message_unref(m);
    sd_bus_slot_unref(sigslot);
    sd_bus_unref(bus);
    return ret < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
