#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define SHMSIZE 1024
#define SHMNAME "/my_posix_shm" 

int main() {
    int shmd;
    void *shmaddr;
    const char *message = "Hello, POSIX Shared Memory";

    // 1. 공유 메모리 객체 생성/개방
    shmd = shm_open(SHMNAME, O_CREAT | O_RDWR, 0666);
    if (shmd == -1) { perror("shm_open failed"); exit(1); }
    printf("Writer: Shared Memory Object 개방 성공 (FD: %d)\n", shmd);

    // 2. 크기 설정
    if (ftruncate(shmd, SHMSIZE) != 0) {
        perror("ftruncate failed"); 
        exit(1); 
    }

    // 3. 프로세스 주소 공간에 매핑
    shmaddr = mmap(NULL, SHMSIZE, PROT_WRITE, MAP_SHARED, shmd, 0);
    if (shmaddr == MAP_FAILED) { perror("mmap failed"); exit(1); }
    printf("Writer: Shared Memory 매핑 성공.\n");

    // 4. 데이터 쓰기
    strcpy((char *)shmaddr, message);
    printf("Writer: Wrote message: '%s'\n", message);
    
    // 5. 매핑 해제 및 닫기
    munmap(shmaddr, SHMSIZE); 
    close(shmd);
    
    return 0;
}
