//
//  main.cpp
//  hw3
//
//  Created by Yedarm Seong on 2017. 11. 3..
//  Copyright © 2017년 Yedarm Seong. All rights reserved.
//

#include <iostream>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#include "WFSnapshot.h"

#define TIME 60

using namespace std;
 
// global snapshot for all thread sharing
WFSnapshot g_snapshot;

void* run_update(void *arg);

int main(int argc, char *argv[]) {
    // first argument: number of thread
    int n_thread = atoi(argv[1]);
    g_snapshot = WFSnapshot(n_thread, 0);
    
    pthread_t threads[n_thread];
    unsigned long long updates[n_thread] = {0, };
    
    for (long i = 0; i < n_thread; ++i) {
        pthread_create(&threads[i], 0, run_update, (void *) i);
    }
    
    unsigned long long all_updates = 0;
    for (int i = 0; i < n_thread; ++i) {
        // thread exit wait
        pthread_join(threads[i], (void **) &updates[i]);
        cout << i << "'th thread updates " << updates[i] << '\n';
        all_updates += updates[i];
    }
    
    cout << "sum of updates " << all_updates << '\n';
    return 0;
}

// 
void* run_update(void *arg) {
    // thread id is create number
    long tid = (long) arg;
    unsigned long long value = 0;
    
    // calculate time
    for (time_t start_time = time(NULL), end_time = start_time + TIME;
         start_time < end_time; start_time = time(NULL))
    {
        g_snapshot.update(++value, (int) tid);
    }
    
    pthread_exit((void *) value);
}
