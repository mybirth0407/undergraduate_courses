#include "hybrid_lock.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

long g_count = 0;

pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_spinlock_t g_spin;
hybrid_lock hybr;

void *thread_func1(void *arg)
{
	long i, j, k, count = (long)arg;
	long long l;

	/*
	 * Increase the global variable, g_count.
	 * This code should be protected by
	 * some locks, e.g. spin lock, and the lock that 
	 * you implemented for assignment,
	 * because g_count is shared by other threads.
	 */
	for (i = 0; i<count; i++) {

		pthread_mutex_lock(&g_mutex);
		/********************** Critical Section **********************/

		/*
		 * The purpose of this code is to occupy cpu for long time.
		 */
		for (j = 0; j<100000; j++)
			for (k = 0; k<3000; k++)
				l += j * k;

		g_count++;
		/**************************************************************/
		pthread_mutex_unlock(&g_mutex);


	}
	return NULL;
}

void *thread_func2(void *arg)
{
	long i, j, k, count = (long)arg;
	long long l;

	/*
	 * Increase the global variable, g_count.
	 * This code should be protected by
	 * some locks, e.g. spin lock, and the lock that 
	 * you implemented for assignment,
	 * because g_count is shared by other threads.
	 */
	for (i = 0; i<count; i++) {

		pthread_spin_lock(&g_spin);
		/********************** Critical Section **********************/

		/*
		 * The purpose of this code is to occupy cpu for long time.
		 */
		for (j = 0; j<100000; j++)
			for (k = 0; k<3000; k++)
				l += j * k;

		g_count++;
		/**************************************************************/
		pthread_spin_unlock(&g_spin);


	}
	return NULL;
}

void *thread_func3(void *arg)
{
	long i, j, k, count = (long)arg;
	long long l;

	/*
	 * Increase the global variable, g_count.
	 * This code should be protected by
	 * some locks, e.g. spin lock, and the lock that 
	 * you implemented for assignment,
	 * because g_count is shared by other threads.
	 */
	for (i = 0; i<count; i++) {

		hybrid_lock_lock(&hybr);
		/********************** Critical Section **********************/

		/*
		 * The purpose of this code is to occupy cpu for long time.
		 */
		for (j = 0; j<100000; j++)
			for (k = 0; k<3000; k++)
				l += j * k;

		g_count++;
		/**************************************************************/
		hybrid_lock_unlock(&hybr);
	}
	return NULL;
}
int main(int argc, char *argv[])
{
	if (3 != argc) {
		fprintf(stderr, "usage: %s <thread count> <value>\n", argv[0]);
		exit(0);
	}
    
    int locktype;
    printf("************************************\n");
    printf("Choose locktype\n");
    printf("  mutex_lock : 1\n  spin_lock : 2\n  hybrid_lock : 3\n");
    printf("************************************\n");
    printf("Enter the locktype : ");
    scanf("%d",&locktype);
    printf("\nlocktype is %d\n",locktype);
    pthread_spin_init(&g_spin, PTHREAD_PROCESS_PRIVATE);
    hybrid_lock_init(&hybr);

    pthread_t *tid;
    long i, thread_count, value;
    int rc;
    /*
     * Check that the program has three arguments
     * If the number of arguments is not 3, terminate the process.
     */

	/*
	 * Get the values of the each arguments as a long type.
	 * It is better that use long type instead of int type,
	 * because sizeof(long) is same with sizeof(void*).
	 * It will be better when we pass argument to thread
	 * that will be created by this thread.
	 */
	thread_count = atol(argv[1]);
	value = atol(argv[2]);

	/*
	 * Create array to get tids of each threads that will
	 * be created by this thread.
	 */
	tid = (pthread_t*)malloc(sizeof(pthread_t)*thread_count);
	if (!tid) {
		fprintf(stderr, "malloc() error\n");
		exit(0);
	}

	/*
	 * Create a threads by the thread_count value received as
	 * an argument. Each threads will increase g_count for
	 * value times.
	 */
	for (i = 0; i<thread_count; i++) {
		if(locktype == 1){
			rc = pthread_create(&tid[i], NULL, thread_func1, (void*)value);
		}
		else if(locktype == 2){
			rc = pthread_create(&tid[i], NULL, thread_func2, (void*)value);
		}
		else if(locktype == 3){
			rc = pthread_create(&tid[i], NULL, thread_func3, (void*)value);
		}
		else{
			printf("LOCKTYPE ERROR\n");
			return 0;
		}

		if (rc) {
			fprintf(stderr, "pthread_create() error\n");
			free(tid);
			pthread_mutex_destroy(&g_mutex);
			exit(0);
		}
	}

	/*
	 * Wait until all the threads you created above are finished.
	 */
	for (i = 0; i<thread_count; i++) {
		rc = pthread_join(tid[i], NULL);
		if (rc) {
			fprintf(stderr, "pthread_join() error\n");
			free(tid);
			pthread_mutex_destroy(&g_mutex);
			exit(0);
		}
	}

	/*
	 * Print the value of g_count.
	 * It must be (thread_count * value)
	 */ 
	printf("value: %ld\n", g_count);

	free(tid);
	pthread_mutex_destroy(&g_mutex);
	pthread_spin_destroy(&g_spin);
	hybrid_lock_destroy(&hybr);

}
