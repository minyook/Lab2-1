#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>     // read, write, isatty, ttyname 등
#include <termios.h>    // tcgetattr, tcsetattr, termios 구조체

#define MAX_SENTENCES 5   // 연습할 문장의 개수
#define MAX_LENGTH 256    // 각 문장의 최대 길이

// 예제 문장들
const char *sentences[MAX_SENTENCES] = {
    "죽는 날까지 하늘을 우러러 한 점 부끄럼이 없기를",
    "잎새에 이는 바람에도 나는 괴로워 했다.",
    "별을 노래하는 마음으로 모든 죽어가는 것을 사랑해야지",
    "그리고 나한테 주어진 길을 걸어가야겠다.",
    "오늘 밤에도 별이 바람에 스치운다."
};

// 터미널 속성 구조체 선언
struct termios saved_attr; 

// 함수 원형
void set_terminal_mode(int enable_canonical);
void typing_practice();
int calculate_errors_realtime(const char *correct_sentence);

/**
 * @brief 메인 함수: 터미널 모드를 설정하고 해제합니다.
 */
int main() {
    printf("타자 연습 (비정규 모드)에 오신 것을 환영합니다!\n");
    printf("--------------------------------------------------------------------\n");
    printf("주의: 엔터 없이 한 글자씩 입력되며, 오타는 '*'로 표시됩니다.\n\n");
    
    // 비정규 모드 설정
    set_terminal_mode(0); // 0 = 비정규 모드 (ICANON 플래그 클리어)

    typing_practice();
    
    // 정규 모드로 복원
    set_terminal_mode(1); // 1 = 정규 모드 (ICANON 플래그 복원)
    
    printf("\n--------------------------------------------------------------------\n");
    printf("타자 연습을 이용해주셔서 감사합니다. 안녕히 가세요!\n");
    return 0;
}

/**
 * @brief 터미널 속성을 설정하거나 복원합니다.
 * @param enable_canonical 0이면 비정규 모드 설정, 1이면 정규 모드 복원
 */
void set_terminal_mode(int enable_canonical) {
    static struct termios new_attr;
    int fd = fileno(stdin); // 표준 입력 파일 기술자 (0)

    if (enable_canonical) {
        // 정규 모드 복원
        if (tcsetattr(fd, TCSANOW, &saved_attr) != 0) {
            perror("tcsetattr restore error");
            exit(1);
        }
        return;
    }

    // 현재 터미널 속성 저장 (저장된 속성은 종료 시 복원됩니다)
    if (tcgetattr(fd, &saved_attr) != 0) {
        perror("tcgetattr error");
        exit(1);
    }
    
    new_attr = saved_attr;
    
    // 비정규 모드 설정 (PDF 80페이지 참고)
    new_attr.c_lflag &= ~ICANON; // 줄 단위 입력 (ICANON) 비활성화
    new_attr.c_lflag &= ~ECHO;   // 입력 문자 화면 출력 (ECHO) 비활성화
    
    // 읽기 제어 모드 설정 (PDF 80페이지 참고)
    new_attr.c_cc[VMIN] = 1;     // 최소 1문자가 입력되어야 read 반환
    new_attr.c_cc[VTIME] = 0;    // 타임아웃 0 (무한 대기)

    if (tcsetattr(fd, TCSANOW, &new_attr) != 0) { // TCSANOW: 즉시 적용
        perror("tcsetattr set error");
        exit(1);
    }
}

/**
 * @brief 전체 문장에 대한 타자 연습을 진행합니다.
 */
void typing_practice() {
    int total_errors = 0;
    double total_time = 0;
    double total_words = 0;

    for (int i = 0; i < MAX_SENTENCES; i++) {
        printf("문장 %d: ", i + 1);
        printf("%s\n", sentences[i]);
        
        // 입력 시작 시간 기록
        time_t start_time = time(NULL);
        
        // 실시간 오타 계산 함수 호출
        int errors = calculate_errors_realtime(sentences[i]);
        
        // 입력 종료 시간 기록
        time_t end_time = time(NULL);
        
        // 시간, 오류 합산 및 출력
        double elapsed_time = difftime(end_time, start_time);
        total_time += (elapsed_time > 0.01 ? elapsed_time : 0.01);
        total_errors += errors;
        total_words += strlen(sentences[i]) / 5.0; // 평균 1단어 5글자 기준
        
        printf("\n오류 수: %d, 소요 시간: %.2f초\n\n", errors, elapsed_time);
    }

    // WPM 계산
    double wpm = (total_words / total_time) * 60;
    
    printf("타자 연습이 완료되었습니다!\n");
    printf("총 오류 수: %d\n", total_errors);
    printf("총 소요 시간: %.2f초\n", total_time);
    printf("평균 분당 타자수(WPM): %.2f\n", wpm);
}

/**
 * @brief 문자를 하나씩 읽으면서 실시간으로 오류를 계산합니다.
 * @param correct_sentence 정확한 문장
 * @return 발생한 총 오류 횟수
 */
int calculate_errors_realtime(const char *correct_sentence) {
    char ch;
    int errors = 0;
    int current_index = 0;
    size_t len_correct = strlen(correct_sentence);

    // 문장 끝('\0') 또는 개행 문자('\n')를 입력할 때까지 반복합니다.
    while (current_index < len_correct) {
        // read(파일기술자, 버퍼, 읽을 바이트 수) -> PDF 17페이지
        if (read(fileno(stdin), &ch, 1) != 1) {
            perror("read error");
            return errors; 
        }

        // Enter 키(개행 문자)를 입력하면 해당 문장 완료
        if (ch == '\n') {
            break;
        }

        // 정확한 문자 비교 (PDF 81페이지의 typingtest.c 논리)
        if (ch == correct_sentence[current_index]) {
            // 맞았을 경우: 해당 문자를 화면에 출력합니다.
            write(fileno(stdout), &ch, 1);
        } else {
            // 틀렸을 경우: '*'를 화면에 출력하고 오류 횟수를 증가시킵니다.
            write(fileno(stdout), "*", 1);
            errors++;
        }
        
        current_index++;
    }
    
    // 만약 문장이 끝나기 전에 Enter를 눌렀다면, 나머지 글자 수를 오류로 처리
    if (current_index < len_correct) {
        errors += (len_correct - current_index);
    }
    
    return errors;
}
