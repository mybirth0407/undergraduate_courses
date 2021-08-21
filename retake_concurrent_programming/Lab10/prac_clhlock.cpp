#include <stdio.h>
#include <pthread.h>

#define NUM_THREAD 8
#define NUM_WORK 1000000

struct node {
    int is_lock;
};
int gap[128];
int cnt_global;

int *tail = NULL;


void lock(int *lock_object, int current, int *pred) {
    *lock_object = true;
    __sync_lock_test_and_set(&tail, current);
    while (*pred == true);
}

void unlock(int *lock_object, int current, int *pred) {
    *lock_object = false;
    current = *pred;
    __sync_synchronize();
}

void* Work(void *args) {
    int current = NULL;
    int pred = NULL;
    for (int i = 0; i < NUM_WORK; ++i) {
        lock(&tail, current, &pred);
        cnt_global++;
        unlock(&tail, current, &pred);
    }
}

int main() {
    pthread_t threads[NUM_THREAD];
    
    for (long i = 0; i < NUM_THREAD; ++i) {
        pthread_create(&threads[i], 0, Work, 0);
    }
    
    for (int i = 0; i < NUM_THREAD; ++i) {
        pthread_join(threads[i], 0);
    }
    
    printf("cnt_global: %d\n", cnt_global);
}
