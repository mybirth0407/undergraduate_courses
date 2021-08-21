#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

#define MAX_LEN 4000

#define ll long long

int **mat_a = NULL;
int **mat_b = NULL;
ll **mat_c = NULL;

void init();
void destroy();
void multi();

int main(void) {
    // 초기화
    struct timeval time;
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
    
    long i, j;
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

    multi();
    fprintf(stdout, "multiply complete!\n");

    ll all_sum = 0;
    for (i = 0; i < MAX_LEN; ++i) {
        for (j = 0; j < MAX_LEN; ++j) {
            all_sum += mat_c[i][j];
        }
    }
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

void multi() {
    int i, j, k;
    ll t_sum;
    for (i = 0; i < MAX_LEN; ++i) {
        for (j = 0; j < MAX_LEN; ++j) {
            t_sum = 0;
            for (k = 0; k < MAX_LEN; ++k) {
                t_sum += (mat_a[i][k] * mat_b[k][j]);
            }
            mat_c[i][j] = t_sum;
        }
    }
}