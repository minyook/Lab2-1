#include <stdio.h>      // printf, perror, snprintf [cite: 1282, 1303]
#include <stdlib.h>     // exit, NULL [cite: 335]
#include <string.h>     // strcmp [cite: 1304]
#include <sys/types.h>  // stat, opendir, dirent 구조체 정의에 필요 [cite: 985, 1400]
#include <sys/stat.h>   // stat 함수, S_ISDIR 매크로 정의 [cite: 821]
#include <unistd.h>     // stat, chdir 등 [cite: 822]
#include <dirent.h>     // DIR, opendir, readdir, closedir 함수와 dirent 구조체 [cite: 986]

// 함수 원형 선언
void list_directory(const char *path);

/**
 * @brief 메인 함수: 시작 경로를 설정하고 디렉토리 나열을 시작합니다.
 */
int main(int argc, char *argv[]) {
    // PDF에 따르면, 기본 경로는 현재 디렉토리(".")입니다.
    const char *start_path = "."; 
    
    // 명령행 인자가 있다면 해당 경로로 시작합니다.
    if (argc > 1) {
        start_path = argv[1];
    }
    
    // 디렉토리 나열 시작
    printf("--- Recursive Directory Listing (ls -R) ---\n");
    list_directory(start_path);
    
    return 0;
}

/**
 * @brief 주어진 경로의 내용물을 나열하고 하위 디렉토리를 재귀적으로 탐색합니다.
 * @param path 탐색할 디렉토리 경로
 */
void list_directory(const char *path) {
    DIR *dir; // 디렉토리 스트림 포인터 
    struct dirent *entry; // 디렉토리 항목 구조체 포인터 

    // 디렉토리를 열기 (opendir) [cite: 987, 991]
    if ((dir = opendir(path)) == NULL) {
        perror("opendir error"); // 오류 발생 시 메시지 출력 [cite: 196]
        return;
    }

    printf("\n%s:\n", path); // 현재 디렉토리 경로 출력

    // --- 1차 순회: 파일/디렉토리 이름 출력 (ls 동작) ---
    while ((entry = readdir(dir)) != NULL) { 
        // 현재 디렉토리(.)와 상위 디렉토리(..)를 건너뜁니다.
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        printf("%s  ", entry->d_name); // 파일/디렉토리 이름 출력
    }
    printf("\n");

    // --- 2차 순회: 하위 디렉토리 탐색 (재귀적 동작) ---
    // 디렉토리 스트림의 위치를 처음으로 되돌립니다. (rewinddir) 
    rewinddir(dir);
    
    while ((entry = readdir(dir)) != NULL) {
        char sub_path[1024];
        struct stat statbuf; // 파일 상태 정보를 저장할 구조체 [cite: 825]
        
        // 현재 디렉토리(.)와 상위 디렉토리(..)를 건너뜁니다.
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // 전체 하위 경로 생성 (예: "current_dir/sub_entry")
        snprintf(sub_path, sizeof(sub_path), "%s/%s", path, entry->d_name);
        
        // 파일 상태를 가져와서 하위 디렉토리인지 확인 (stat, S_ISDIR) [cite: 793, 836]
        // stat() 대신 lstat()을 사용하여 심볼릭 링크를 따라가지 않도록 하는 것이 일반적이지만,
        // 여기서는 stat()을 사용하여 PDF 예제에 맞춥니다.
        if (stat(sub_path, &statbuf) == 0 && S_ISDIR(statbuf.st_mode)) {
            // 재귀적으로 하위 디렉토리 탐색
            list_directory(sub_path);
        }
    }
    
    // 디렉토리 닫기 (closedir) [cite: 992]
    closedir(dir);
}
