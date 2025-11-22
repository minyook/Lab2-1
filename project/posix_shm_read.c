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

    // 1. 공유 메모리 객체 개방
    shmd = shm_open(SHMNAME, O_RDWR, 0666);
    if (shmd == -1) { perror("shm_open failed"); exit(1); }

    // 2. 프로세스 주소 공간에 매핑
    shmaddr = mmap(NULL, SHMSIZE, PROT_READ, MAP_SHARED, shmd, 0);
    if (shmaddr == MAP_FAILED) { perror("mmap failed"); exit(1); }

    // 3. 데이터 읽기
    printf("Reader: Received message: %s\n", (char *)shmaddr);

    // 4. 매핑 해제 및 닫기
    munmap(shmaddr, SHMSIZE);
    close(shmd);
    
    // 5. 공유 메모리 객체 삭제
    if (shm_unlink(SHMNAME) == -1) {
        perror("shm_unlink failed");
        exit(1); 
    }
    
    return 0;
}
