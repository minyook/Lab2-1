#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>

void mmap_copy(const char *src_path, const char *dst_path);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <source_file> <destination_file>\n", argv[0]);
        return 1;
    }

    mmap_copy(argv[1], argv[2]);

    return 0;
}

/**
 * @brief mmap을 사용하여 소스 파일을 대상 파일로 복사합니다.
 * @param src_path 소스 파일 경로
 * @param dst_path 대상 파일 경로
 */
void mmap_copy(const char *src_path, const char *dst_path) {
    int src_fd, dst_fd;
    struct stat sb;
    off_t file_size;
    void *src_map, *dst_map;

    // 1. 소스 파일 열기 및 크기 확인
    src_fd = open(src_path, O_RDONLY);
    if (src_fd == -1) {
        perror("Error opening source file");
        return;
    }
    
    if (fstat(src_fd, &sb) == -1) { // 파일 상태 정보 획득
        perror("Error getting source file size");
        close(src_fd);
        return;
    }
    file_size = sb.st_size;

    // 2. 대상 파일 생성 및 크기 설정
    dst_fd = open(dst_path, O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (dst_fd == -1) {
        perror("Error opening destination file");
        close(src_fd);
        return;
    }
    
    // 대상 파일 크기를 소스 파일 크기와 동일하게 설정 (mmap을 위해 필수)
    if (ftruncate(dst_fd, file_size) == -1) { 
        perror("Error setting destination file size");
        close(src_fd);
        close(dst_fd);
        return;
    }

    // 3. 소스 파일을 읽기 전용으로 메모리에 매핑
    // PROT_READ, MAP_SHARED 설정 (PDF 13페이지 참고)
    src_map = mmap(NULL, file_size, PROT_READ, MAP_SHARED, src_fd, 0);
    if (src_map == MAP_FAILED) {
        perror("Error mapping source file");
        close(src_fd);
        close(dst_fd);
        return;
    }

    // 4. 대상 파일을 쓰기 가능하도록 메모리에 매핑
    dst_map = mmap(NULL, file_size, PROT_WRITE, MAP_SHARED, dst_fd, 0);
    if (dst_map == MAP_FAILED) {
        perror("Error mapping destination file");
        munmap(src_map, file_size);
        close(src_fd);
        close(dst_fd);
        return;
    }
    
    // 5. 메모리 대 메모리 복사 (실제 파일 복사 작업)
    // 메모리 연산으로 I/O를 대체하여 복사 속도가 빠릅니다.
    memcpy(dst_map, src_map, file_size);
    printf("File copied successfully (Size: %ld bytes).\n", file_size);

    // 6. 매핑 해제 및 파일 닫기 (munmap)
    munmap(src_map, file_size); // 메모리 매핑 삭제 [cite: 3487]
    munmap(dst_map, file_size);
    close(src_fd);
    close(dst_fd);
}
