#include <stdio.h>
#include <pthread.h>

#define NUM_THREAD 10
#define NUM_INCREASE 1000000

int cnt_global = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* ThreadFunc(void *arg) {
    long cnt_local = 0;
    
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < NUM_INCREASE; ++i) {
        cnt_global++;
        cnt_local++;
    }
    pthread_mutex_unlock(&mutex);

	return (void *) cnt_local;
}

int main() {
	pthread_t threads[NUM_THREAD];

	// create threads
	for (int i = 0; i < NUM_THREAD; ++i) {
		if (pthread_create(&threads[i], 0, ThreadFunc, NULL) < 0) {
			printf("pthread create error!\n");
			return 0;
		}
	}
	// wait threads end
	long ret;

	for (int i = 0; i < NUM_THREAD; ++i) {
		pthread_join(threads[i], (void **) &ret);
		printf("thread %lu, local count: %ld\n", threads[i], ret);		
	}
	printf("global count: %d\n", cnt_global);
}