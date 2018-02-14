#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

#define MAX_LEN 4000

#define ll long long

int **mat_a = NULL;
int **mat_b = NULL;
ll **mat_c = NULL;

int num_seg;
int rem;

void init();
void* thread_func(void *arg);
void destroy();

int main(int argc, char *argv[]) {
    // 인자 개수 테스트
    if (argc != 2) {
        fprintf(stderr, "number of argument error!\n");
        fprintf(stderr, "Usage: ./2.3 [number of thread]\n");
        exit(-1);
    }
    else {
        fprintf(stdout, "number of argument test pass!\n");
    }

    // 인자 테스트 
    int num_thread = atoi(argv[1]);
    if (num_thread <= 0) {
        fprintf(stderr, "number of thread error!\n");
        fprintf(stderr, "number of thread should be greater than zero!\n");
        exit(-1);
    }
    else {
        fprintf(stdout, "number of thread test pass!\n");
    }
    // 초기화
    struct timeval time;
    pthread_t thread[num_thread];
    ll sum[num_thread];
    num_seg = MAX_LEN / num_thread;
    rem = MAX_LEN % num_thread;
    init();

    // 파일 읽기
    FILE *sample1;
    sample1 = fopen("./sample1.txt", "r");
    if (NULL == sample1) {
        fprintf(stderr, "sample 1 read error!\n");
    }

    FILE *sample2;
    sample2 = fopen("./sample2.txt", "r");
    if (NULL == sample2) {
        fprintf(stderr, "sample 2 read error!\n");
    }
    
    long i, j, k;
    for (i = 0; i < MAX_LEN; ++i) {
        for (j = 0; j < MAX_LEN; ++j) {
            fscanf(sample1, "%d", &mat_a[i][j]);
            fscanf(sample2, "%d", &mat_b[i][j]);
        }
    }
    fclose(sample1);
    fclose(sample2);
    
    // 여기서부터 수행 시간을 측정한다.
    gettimeofday(&time, NULL);
    time_t start_time = time.tv_sec;

    for (i = 0; i < num_thread; ++i) {
        if (0 > pthread_create(&thread[i], NULL, thread_func, (void *) i)) {
            fprintf(stderr, "thread create error!\n");
            exit(-1);
        }
    }

    // 나머지
    ll t_sum;
    for (i = MAX_LEN - rem; i < MAX_LEN; ++i) {
        for (j = 0; j < MAX_LEN; ++j) {
            t_sum = 0;
            for (k = 0; k < MAX_LEN; ++k) {
                t_sum += (mat_a[i][k] * mat_b[k][j]);
            }
            mat_c[i][j] = t_sum;
        }
    }

    ll seg_sum = 0;
    for (i = MAX_LEN - rem; i < MAX_LEN; ++i) {
        for (j = 0; j < MAX_LEN; ++j) {
            seg_sum += mat_c[i][j];
        }
    }

    for (i = 0; i < num_thread; ++i) {
        if (0 != pthread_join(thread[i], NULL)) {
            fprintf(stderr, "thread join error!\n");
            exit(-1);
        }
    }

    ll all_sum = seg_sum;
    for (i = 0; i < MAX_LEN; ++i) {
        for (j = 0; j < MAX_LEN; ++j) {
            all_sum += mat_c[i][j];
        }
    }
    fprintf(stdout, "sum of all element complete!\n");

    gettimeofday(&time, NULL);
    time_t end_time = time.tv_sec;
    // 수행 시간을 출력한다.
    fprintf(stdout, "calculate time: %ld\n", end_time - start_time);
    // 결과값의 합을 출력한다.
    fprintf(stdout, "sum of matrix elements: %lld\n", all_sum);

    destroy();
    return 0;
}

void init() {
    int i;
    mat_a = (int **) malloc(sizeof(int *) * MAX_LEN);
    for (i = 0; i < MAX_LEN; ++i) {
        mat_a[i] = (int *) malloc(sizeof(int) * MAX_LEN);
    }

    mat_b = (int **) malloc(sizeof(int *) * MAX_LEN);
    for (i = 0; i < MAX_LEN; ++i) {
        mat_b[i] = (int *) malloc(sizeof(int) * MAX_LEN);
    }

    mat_c = (ll **) malloc(sizeof(ll *) * MAX_LEN);
    for (i = 0; i < MAX_LEN; ++i) {
        mat_c[i] = (ll *) malloc(sizeof(ll) * MAX_LEN);
    }
}

void destroy() {
    // 할당받았던 모든 동적 메모리를 해제한다.
    int i;
    for (i = 0; i < MAX_LEN; ++i) {
        free(mat_a[i]);
    }
    free(mat_a);

    for (i = 0; i < MAX_LEN; ++i) {
        free(mat_b[i]);
    }
    free(mat_b);

    for (i = 0; i < MAX_LEN; ++i) {
        free(mat_c[i]);
    }
    free(mat_c);
}

void* thread_func(void *arg) {
    long thread_num = (long) arg;
    // 쓰레드 진입시 출력
    fprintf(stdout, "thread %ld entered!\n", thread_num);

    int i, j, k;
    ll t_sum;

    // 가장 단순한 행렬 곱
    for (i = thread_num * num_seg; i < (thread_num + 1) * num_seg; ++i) {
        for (j = 0; j < MAX_LEN; ++j) {
            t_sum = 0;
            for (k = 0; k < MAX_LEN; ++k) {
                t_sum += (mat_a[i][k] * mat_b[k][j]);
            }
            mat_c[i][j] = t_sum;
        }
    }
    // 쓰레드가 주어진 행렬 곱 연산을 완료하였을 때 출력
    fprintf(stdout, "thread %ld multiply complete!\n", thread_num);
}
