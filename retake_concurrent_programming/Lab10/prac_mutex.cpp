#include <stdio.h>
#include <pthread.h>

#define NUM_THREAD 10
#define NUM_INCREASE 1000000

int cnt_global = 0;
bool flag[NUM_THREAD * NUM_INCREASE];

void* ThreadFunc(void *arg) {
    int ticket;
	for (int i = 0; i < NUM_INCREASE; ++i) {
        ticket = __sync_fetch_and_add(&cnt_global, 1);
        flag[ticket] = true;
	}
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
		pthread_join(threads[i], (void **)&ret)
    }
    
    int i;
    for (i = 0; i < NUM_THREAD * NUM_INCREASE; ++i) {
        if (flag[i] == false) {
            printf("ERROR\n");
            break;
        }
    }
    
    if (i == NUM_THREAD * NUM_INCREASE) {
        printf("ALL FLAGS ON\n");
    }
	printf("gloal count: %d\n", cnt_global);

	return 0;
}