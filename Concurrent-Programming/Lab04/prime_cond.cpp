#include <stdio.h>
#include <pthread.h>
#include <math.h>

#define NUM_THREAD  10

int thread_ret[NUM_THREAD];

int range_start;
int range_end;

int thread_sleep;
int thread_finish;

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

bool flag;


bool IsPrime(int n) {
    if (n < 2) {
        return false;
    }

    for (int i = 2; i <= sqrt(n); i++) {
        if (n % i == 0) {
            return false;
        }
    }
    return true;
}

void* ThreadFunc(void* arg) {
    while (!flag) {
        pthread_mutex_lock(&mutex);
        thread_sleep += 1;
        pthread_cond_wait(&cond, &mutex);
        pthread_mutex_unlock(&mutex);

        long tid = (long)arg;
        
        // Split range for this thread
        int start = range_start + ((range_end - range_start) / NUM_THREAD) * tid;
        int end;
        if (tid == NUM_THREAD - 1) {
            end = range_end + 1;
        }
        else {
            end = range_start + ((range_end - range_start) / NUM_THREAD) * (tid+1);
        }
        
        long cnt_prime = 0;
        for (int i = start; i < end; i++) {
            if (IsPrime(i)) {
                cnt_prime++;
            }
        }
        thread_ret[tid] = cnt_prime;
        
        // pthread_mutex_lock(&mutex);
        // thread_finish += 1;
        // pthread_mutex_unlock(&mutex);
        __sync_fetch_and_add(&thread_finish, 1);
    }

    return NULL;
}

int main(void) {
    pthread_t threads[NUM_THREAD];
    thread_sleep = 0;
    thread_finish = 0;
   
    flag = false;
    
    // Create threads to work
    for (long i = 0; i < NUM_THREAD; i++) {
        if (pthread_create(&threads[i], 0, ThreadFunc, (void*)i) < 0) {
            printf("pthread_create error!\n");
            return 0;
        }
    }
    
    while (true) {
        // Input range
        scanf("%d", &range_start);
        if (range_start == -1) {
            flag = true;
            pthread_cond_broadcast(&cond);
            break;
        }
        scanf("%d", &range_end);
        

        while (thread_sleep < 10);
        thread_sleep = 0;
        pthread_mutex_lock(&mutex);
        pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&mutex);
        while (thread_finish < 10);
        thread_finish = 0;

        // Collect results
        int cnt_prime = 0;

        for (int i = 0; i < NUM_THREAD; i++) {
            cnt_prime += thread_ret[i];
        }
        printf("number of prime: %d\n", cnt_prime);
        
    }
 
    // Wait threads end
    for (int i = 0; i < NUM_THREAD; i++) {
        pthread_join(threads[i], NULL);
    }
    return 0;
}

