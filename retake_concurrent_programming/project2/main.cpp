//
//  main.cpp
//  Simple Two-phase locking with Readers-writer Lock
//
//  Created by Yedarm Seong on 2017. 10. 2..
//  Copyright © 2017년 Yedarm Seong. All rights reserved.
//

// ./run 3 5 100 3


#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#include <sys/types.h>
#include <sys/syscall.h>

#include <iterator>
#include <algorithm>
#include <list>
#include <stack>

#include <fstream>
#include <iostream>

using namespace std;

// Define READ and WRITE, not exsit unlock state
typedef enum {READ, WRITE} lock_type;
typedef enum {WATING, RUNNING} lock_state;

int n_thread;
int n_record;

unsigned long long g_max_order;
unsigned long long g_execution_order = 0;

pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t g_cond = PTHREAD_COND_INITIALIZER;

// Record structure
struct record_t {
    long long value_ = 100;
};

// Readers-Writer lock structure
struct rw_lock_t {
    lock_type type_;
    lock_state state_;
    int tid_;
    
    // member variable setters
    void set_type(lock_type type) {
        type_ = type;
    }
    void set_tid(int tid) {
        tid_ = tid;
    }
    void set_state(lock_state state) {
        state_ = state;
    }
};

// Node for lock table
struct node_t {
    // counting lock
    long rdlock_cnt_ = 0;
    long wrlock_cnt_ = 0;
    
    list <struct rw_lock_t> lock_list;
    
    void increase_rdlock_cnt() {
        rdlock_cnt_ += 1;
    }
    void decrease_rdlock_cnt() {
        rdlock_cnt_ -= 1;
    }
    
    void increase_wrlock_cnt() {
        wrlock_cnt_ += 1;
    }
    void decrease_wrlock_cnt() {
        wrlock_cnt_ -= 1;
    }
};

// Thread function for run transaction
void* run_trx(void *arg);
// Set three random numbers to rand_nums variable
void set_rand_nums(int n_record, int *rand_nums);
// Split readers and writer locks
void acquire_rdlock(rw_lock_t lock, int tid, int record);
void acquire_wrlock(rw_lock_t lock, int tid, int record);
// Integrated unlock, lock type is checking
void release_lock(int tid, int record, lock_type t);
// Release all readers-writer lock this transaction
void release_trx_lock(int i, int j, int k, int tid);
// Undo operationc, if transaction run failed
void rollback_records(int i, int j, int k,
                      long long i_value, long long j_value, long long k_value);
// Readers and writerrs may be in deadlock situations.
bool deadlock_checking(int level, int tid, int target, stack<int> &run_list,
                       int i, int j, int k, int my_idx);

struct record_t *records;
struct node_t *lock_table;


int main(int argc, char *argv[]) {
    // Radnom function seed
    srand((unsigned int) time(NULL));
    
    // argument error, required only 3 arguments
    if (argc != 4) {
        fputs("Usage: ./run [n] [r] [e]", stderr);
        return -1;
    }
    
    // argument parsing
    n_thread = atoi(argv[1]);
    n_record = atoll(argv[2]) ;
    g_max_order = atoll(argv[3]);

    // argument value error
    if (n_thread <= 0) {
        fputs("number of thread should be greater than 0",
              stderr);
        return -1;
    }
    else if (n_record < 3) {
        fputs("number of records should be at least greater than 3",
              stderr);
        return -1;
    }
    else if (g_max_order <= 0) {
        fputs("global execution order threshold should be greater than 0",
              stderr);
        return -1;
    }

    pthread_t threads[n_thread];
    // Global variable initialize
    lock_table = new node_t[n_record];
    records = new record_t[n_record];
    
    // thread create, argument is thread create order
    for (long i = 0; i < n_thread; ++i) {
        if (pthread_create(&threads[i], NULL, run_trx, (void *) i) < 0) {
            exit(0);
        }
    }
    
    for (int i = 0; i < n_thread; ++i) {
        pthread_join(threads[i], NULL);
    }
    
    // Dynamic allocation memory free
    delete lock_table;
    delete records;
    
    return 0;
}

/**
 * Run transaction funtion
 * arg : thread order for using tid
 */
void* run_trx(void *arg) {
    // thread number
    int tid = (int) ((long) arg + 1);
    
    // rdlock for i, wrlock for j, k
    rw_lock_t lock;
    lock.set_tid(tid);
    int rand_nums[3];
    
    int i, j, k;
    unsigned long long commit_id;
    long long read_value;
    long long i_value;
    long long j_value;
    long long k_value;
    
    ofstream logfile("thread" + to_string(tid) + ".txt");
    
    while (g_execution_order < g_max_order) {
        // 1 random select three records
        set_rand_nums(n_record, rand_nums);
        i = rand_nums[0];
        j = rand_nums[1];
        k = rand_nums[2];
        
        // 2 acquire global lock
        pthread_mutex_lock(&g_lock);
        // save for rollback
        i_value = records[i].value_;
        j_value = records[j].value_;
        k_value = records[k].value_;
        
        // 3 acquire read lock for get i's record value
        acquire_rdlock(lock, tid, i);
        // 4 release global lock
        pthread_mutex_unlock(&g_lock);
        // 5 get i's record value
        read_value = records[i].value_;
        // 6 acquire global lock
        pthread_mutex_lock(&g_lock);
        // 7 acquire write lock for get j's record value
        stack<int> runnings;
        acquire_wrlock(lock, tid, j);

        if (deadlock_checking(0, tid, tid, runnings, i, j, k, 1)) {
            rollback_records(i, j, k, i_value, j_value, k_value);
            release_trx_lock(i, j, k, tid);
            pthread_mutex_unlock(&g_lock);
            continue;
        }
        
        // all element remove
        while(!runnings.empty()) {
            runnings.pop();
        }
        // 8 release global lock
        pthread_mutex_unlock(&g_lock);

        // 9 record j's value = record i's value + 1
        records[j].value_ = j_value + read_value + 1;
        // 10 acquire global lock
        pthread_mutex_lock(&g_lock);
        // 11 acquire write lock for write k's record value
        acquire_wrlock(lock, tid, k);

        if (deadlock_checking(0, tid, tid, runnings, i, j, k, 2)) {
            rollback_records(i, j, k, i_value, j_value, k_value);
            release_trx_lock(i, j, k, tid);
            pthread_mutex_unlock(&g_lock);
            continue;
        }
        // 12 release global lock
        pthread_mutex_unlock(&g_lock);
        // 13 record k's value = record k's value - record i's value
        records[k].value_ -= read_value;
        // commit
        
        // 14 acquire global lock
        pthread_mutex_lock(&g_lock);
        // 15 release all rw lock this transcation
        release_trx_lock(i, j, k, tid);
        // 16
        // commit id = ++global execution order
        commit_id = ++g_execution_order;
        if (commit_id > g_max_order) {
            // undo(rollback)
            rollback_records(i, j, k, i_value, j_value, k_value);
            // release global lock
            pthread_mutex_unlock(&g_lock);
            // thread exit
            break;
        }
        
        printf("commit id is %llu\n", commit_id);
        // 17 commit log append
        // [commit_id] [i] [j] [k] [value of record i] [value of record j] [value of record k]
        logfile << commit_id << ' ' << i << ' ' << j << ' ' << k << ' ';
        logfile << records[i].value_ << ' ' << records[j].value_ << ' ' << records[k].value_ << endl;
        
        // 18 release global lock
        pthread_mutex_unlock(&g_lock);
    }
    exit(0);
}


void set_rand_nums(int n_record, int rand_nums[3]) {
    while (true) {
        // generate i
        rand_nums[0] = rand() % n_record;
        while (true) {
            // generate j
            rand_nums[1] = rand() % n_record;
            // i != j
            if (rand_nums[0] != rand_nums[1]) {
                // generate k
                rand_nums[2] = rand() % n_record;
                // i != k, j != k
                if ((rand_nums[0] != rand_nums[2])
                    && (rand_nums[1] != rand_nums[2]))
                {
                    return;
                }
            }
        }
    }
}


bool deadlock_checking(int level, int tid, int target, stack<int> &run_list,
                       int i, int j, int k, int my_idx)
{    
    // Occur cycle!
    if (level != 0 && target == tid) {
        return true;
    }
    
    // if this transaction's lock is running, then not exist cycle
    if (lock_table[my_idx].lock_list.back().state_ == RUNNING) {
        return false;
    }

    // begin == 0, end != latest index
    int cnt = 0;
    for (auto it = lock_table[my_idx].lock_list.begin();
        it != lock_table[my_idx].lock_list.end(); ++it)
    {
        // this lock is running, then no exist cycle 
        if ((*it).state_ == RUNNING && (*it).tid_ == target) {
            // undo this loop
            for (int a = 0; a < cnt; ++a) {
                run_list.pop();
            }
            return false;
        }

        // make running lock list
        if ((*it).state_ == RUNNING) {
            run_list.push((*it).tid_);
            ++cnt;
        }
    }

    // running list test
    while (!run_list.empty()) {
        // want search this thread's lock
        int target2 = run_list.top();
        run_list.pop();
        
        for (int idx = 0; idx < 3; ++idx) {
            // my idx is running this transaction
            if (idx == my_idx) {
                continue;
            }
            
            for (auto it = lock_table[idx].lock_list.begin();
                it != lock_table[idx].lock_list.end(); ++it)
            {
                if ((*it).tid_ == target2 && (*it).state_ == WATING) {
                    bool ret = deadlock_checking(
                        level + 1, tid, target2, run_list, i, j, k, idx);
                    if (ret) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

// Readers Locking
void acquire_rdlock(rw_lock_t lock_, int tid, int record) {
    rw_lock_t lock;
    lock.set_type(READ);

    // this record's first lock 
    if (lock_table[record].lock_list.empty()) {
        lock.set_state(RUNNING);
    }
    else {
        if (lock_table[record].lock_list.back().type_ == WRITE) {
            lock.set_state(WATING);
        }
        else {
            lock.set_state(lock_table[record].lock_list.back().state_);
        }
    }
    // lock table add, counting update
    lock_table[record].lock_list.push_back(lock);
    lock_table[record].increase_rdlock_cnt();
}

void acquire_wrlock(rw_lock_t lock_, int tid, int record) {
    rw_lock_t lock;
    lock.set_type(WRITE);
    
    if (lock_table[record].lock_list.empty()) {
        lock.set_state(RUNNING);
    }
    else {
        lock.set_state(WATING);
    }

    // lock table add, counting update
    lock_table[record].lock_list.push_back(lock);
    // while (lock_table[record].lock_list.front().tid_ != tid) {
    //     pthread_cond_wait(&g_cond, &g_lock);
    // }
    lock_table[record].increase_wrlock_cnt();
}

// type check release
void release_lock(int tid, int record, lock_type t) {
    for (auto it = lock_table[record].lock_list.begin();
        it != lock_table[record].lock_list.end(); )
    {
        if ((*it).tid_ == tid && (*it).type_ == t) {
            it = lock_table[record].lock_list.erase(it);
            // before release lock, running
            if (t == READ) {
                for (auto it2 = lock_table[record].lock_list.begin();
                    it2 != it;)
                {
                    (*it2).state_ = RUNNING;
                    ++it2;
                }
                lock_table[record].decrease_rdlock_cnt();
            }
            else if (t == WRITE) {
                lock_table[record].lock_list.front().state_ = RUNNING;
                lock_table[record].decrease_wrlock_cnt();
            }
            // if (lock_table[record].wrlock_cnt_ == 0) {
            //     pthread_cond_broadcast(&g_cond);
            // }
            break;
        }
        else {
            ++it;
        }
    }
}

// all lock release
void release_trx_lock(int i, int j, int k, int tid) {
    release_lock(tid, i, READ);
    release_lock(tid, j, WRITE);
    release_lock(tid, k, WRITE);
}

// temp variable change
void rollback_records(int i, int j, int k,
                      long long i_value, long long j_value, long long k_value)
{
    records[i].value_ = i_value;
    records[j].value_ = j_value;
    records[k].value_ = k_value;
}
