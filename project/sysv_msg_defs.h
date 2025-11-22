#define BUFSIZE 16
#define QKEY (key_t)0x1234 

struct msgq_data {
    long type;      
    char text[BUFSIZE];
};
