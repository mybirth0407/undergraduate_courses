#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <signal.h>

#define ll long long
#define ull unsigned long long

#define true 1
#define false 0

ull g_count;
const char* file_name;
pid_t child_pid[3];

// struct sigaction act_p;
// struct sigaction act_c;

// 자식이 부모에게 보내는 시그널
static void sig_c(int signo);
// 부모가 자식에게 보내는 시그널
static void sig_p(int signo);
// 자식 프로세스가 실행할 함수
static void wait_signal();


int main(int argc, char **argv) {
    ull cnt;
    // 프로그램 인자 확인
    if (3 != argc) {
        fprintf(stderr, "Usage: [count] [file name]\n");
        fprintf(stderr, "Ex) ./count 100000 sample.txt\n");
        exit(1);
    }
    
    // 첫번째 인자 확인
    g_count = atoi(argv[1]);
    if (0 > g_count) {
        fprintf(stderr, "count is should be greater than zero\n");
    }
    
    // 두번째 인자 확인
    file_name = argv[2];
    if (NULL == file_name) {
        fprintf(stderr, "file name is should be required\n");
    }
    
    // 부모가 보낼 시그널을 핸들러에 등록한다.
    signal(SIGUSR2, sig_p);
    
    // 파일이 이미 존재한다면 읽기 모드로 연다.
    FILE *fp = fopen(file_name, "r");
    // 파일이 존재하지 않는다면 파일을 생성한다.
    while (NULL == fp) {
        fp = fopen(file_name, "w+");
        fprintf(fp, "0");
        fseek(fp, 0, SEEK_SET);
    }
    
    // 파일에는 0 이상의 숫자만 쓰여야 한다.
    if (fscanf(fp, "%llu", &cnt) < 0) {
        fprintf(stderr, "get count number error\n");
        exit(1);
    }
    
    // 파일에 쓰여진 숫자가 0이 아니라면 0으로 덮어씌운다.
    if (0 != cnt) {
        fclose(fp);
        fp = NULL;
        
        while (NULL == fp) {
            fp = fopen(file_name, "w+");
            fprintf(fp, "0");
            fseek(fp, 0, SEEK_SET);
        }
    }
    
    // 파일초기화 후 파일 닫기
    fclose(fp);
    
    // 3개의 자식을 생성하고, 각 자식은 부모가 신호를 발생하기를 기다린다.
    int i;
    for (i = 0; i < 3; ++i) {
        if (0 == (child_pid[i] = fork())) {
            wait_signal();
        }
    }
    
    // 자식을 깨우기 위해 자신에게 시그널은 보낸다.
    kill(getpid(), SIGUSR2);
    
    // 자식의 시그널을 대기한다.
    while (true) {
        pause();
    }
    return 0;
}

void wait_signal() {
    signal(SIGUSR1, sig_c);
    
    while (true) {
        pause();
    }
}

void sig_c(int signo) {
    if (signo != SIGUSR1) {
        fprintf(stderr, "signal error!\n");
        return;
    }
    pid_t ppid = getppid();
    FILE *fp;
    int count;
    
    fp = fopen(file_name, "r+");
    
    if (fscanf(fp, "%d", &count) < 0) {
        fprintf(stderr, "get count number error!\n");
        exit(1);
    }

    // 파일의 커서를 파일의 처음 위치로 바꾼다.
    fseek(fp, 0, SEEK_SET);

    // 파일에서 읽은 숫자에 1을 더한 후 파일에 덮어쓴다.
    ++count;
    // 1을 더했는데 0과 같으면 기존의 숫자는 음수기 때문에 오류이다.
    if (fprintf(fp, "%d", count) <= 0) {
        fprintf(stderr, "set count number error!\n");
        exit(1);
    }

    // 열려있는 파일 닫기
    fclose(fp);

    // 부모 프로세스에게 한번의 작업이 완료되었음을 알린다.
    kill(ppid, SIGUSR2);
}

void sig_p(int signo) {
    if (signo != SIGUSR2) {
        fprintf(stderr, "signal error!\n");
        return;
    }
    
    // 선택할 자식의 순서
    static int select = 0;
    int i;

    // 
    if (g_count-- <= 0) {
        // count만큼 해당 신호를 받았다는 것은
        // 원하는 목적을 달성했다고 볼 수 있다.
        for (i = 0; i < 3; ++i) {
            kill(child_pid[i], SIGKILL);
        }
        // 목적한 작업의 수행을 마친 후 부모 프로세스도 종료한다.
        exit(0);
    }
    else {
        kill(child_pid[select], SIGUSR1);
        ++select;
        select %= 3;
    }
}