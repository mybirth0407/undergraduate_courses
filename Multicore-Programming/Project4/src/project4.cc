/* Multicore Programming Project 4
 * Simple & Fast Transaction Locking System
 *
 * 2015.12.09 ~ 2015.12.20
 *
 * Code by Yedarm Seong
 */

/* C header file */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <getopt.h>
#include <unistd.h>
#include <pthread.h>

/* C++ header file */
#include <iostream>
#include <vector>

/* For std::vector use */
using namespace std;

/* enum transaction_state, struct lock_t and trx_t are given specifications
 * RUNNING - active, IDLE - committed */
enum transaction_state { RUNNING, WAITING, IDLE };
enum lock_state { NONE, SHARED, EXCLUSIVE };

/* First declare struct for use struct immediately */
struct trx_t;
struct lock_t;
struct program_argument_t;
struct record_t;
struct lock_hash_table_t;
struct thread_argument_t;

/* Detailed declare struct trx_t, lock_t */
struct
trx_t {
    unsigned long trx_id;
    vector <lock_t *> trx_locks;
    transaction_state trx_state;
    pthread_rwlock_t trx_rwlock;
    lock_t *wait_lock;
    lock_hash_table_t *lock;
};

struct
lock_t {
    unsigned long table_id;
    unsigned long record_id;
    /* RECORD_LOCK_SHARED(0) or RECORD_LOCK_EXCLUSIVE(1) */
    bool lock_mode;
    struct trx_t *trx;
    unsigned long trx_id;
    int lock_state;
    unsigned long timestamp;
    struct lock_t *next;
};

/* struct for parsing arguments */
struct
program_argument_t {
    int table_size;
    int num_thread;
    int read_num;
    int duration;
};

/* struct for record list */
struct
record_t {
    unsigned long long record_id;
    unsigned long long record_value;
    int last_update_trx;
    struct record *next;
};

/* hash rule is
 * table_a = 100000 + record_id
 * table_b = record_id */
struct
lock_hash_table_t {
    lock_state lck_state;
};

struct thread_argument_t {
    int trx_id;
    int num_thread;
    int table_size;
    int read_num;
};

/* For get current transaction id */
unsigned long g_counter;
/* Global counters */
unsigned long g_conflict;
unsigned long g_read;
unsigned long g_update;
unsigned long g_abort;

/* Global dummy pointer for record table A, B */
struct record_t *g_table_a;
struct record_t *g_table_b;

/* Global pointer for lock_hash_table */
struct lock_hash_table_t *g_lck_hash_table;

/* Global variable for program argumnet access */
struct program_argument_t program_argument;

/* Debug variable */
int debug;
void DEBUG(int line_num) {
    __sync_fetch_and_add(&debug, 1);
    char s[50];
    sprintf (s, "%d <- %d", debug, line_num);
    printf("%s\n", s);
}

/* Start time check */
time_t start_time;

/* Parsing arguments in comman line,
 * return value is success to false, fail to true */
bool ParsingArguments(int argc, char *argv[]);

/* Function to thread operating */
void* ThreadFunc(void *arg);

void CreateLockHashTable(int table_size);
void CreateRecordTable(int table_size);
void Summary();

int
main(int argc, char *argv[]) {
  
    /* For rand() function seed */
    srand(time(NULL));
    /* Debug count */
    debug = 0;
    /* Global counters initilize */
    g_counter = 1;
    g_read = 0;
    g_update = 0;
    g_abort = 0;
    /* Program argument default */
    program_argument.table_size = 10000;
    /* sysconf system call is return number of cores */
    program_argument.num_thread = (int) sysconf(_SC_NPROCESSORS_ONLN);
    program_argument.read_num = 10;
    program_argument.duration = 30;
    
    /* Check to pthread_create(), pthread_join */
    int err;
    /* Pass to thread routine */
    int *pass_trx_number;
    
    /* Time check, start */
    time(&start_time);

    /* Program arguments parsing, success is return false */
    if (ParsingArguments(argc, argv) == true) {
        puts("Arguments parsing is not successed, program excute failed");
    }
    
    printf("ParsingArguments! "
            "table_size: %d num_thread: %d read_num: %d duration: %d\n",
            program_argument.table_size, program_argument.num_thread, 
            program_argument.read_num, program_argument.duration);
 
    /* Table create */
    CreateRecordTable(program_argument.table_size);
    CreateLockHashTable(program_argument.table_size);
    
    /* For pthread_create */
    pthread_t *pthread = (pthread_t *) malloc(
            sizeof(pthread_t) * program_argument.num_thread);
    
    /* Thread routine start */
    for (int i = 0; i < program_argument.num_thread; i++) {
        err = pthread_create(
                &pthread[i], NULL, ThreadFunc, NULL);
        
        /* Thread create error */
        if (err < 0) {
            puts("pthread_create failed!");
            exit(0);
        }
    }
    
    /* Wating all thread return */
    for (int i = 0; i < program_argument.num_thread; i++) {
        err = pthread_join(pthread[i], NULL);
        
        /* Thread join error */
        if (err < 0) {
            puts("pthread_join failed!");
            exit(0);
        }
    }

    Summary();

    /* Memory allocate free */
    free(pthread);
    free(pass_trx_number);

    return 0; 
}


bool
ParsingArguments(int argc, char *argv[]) {
    /* Variable for getopt_long */
    int c;
    /* flag for arguments parsing successed */
    bool flag = false;
    /* getopt loop for arguments parsing */
    while (true) {
        static struct option options[] = {
            { "table_size", required_argument, NULL, 't' },
            { "num_thread", required_argument, NULL, 'n' },
            { "read_num", required_argument, NULL, 'r' },
            { "duration", required_argument, NULL, 'd' },
            { 0, 0, 0, 0 }
        };

        c = getopt_long(argc, argv, "t:n:r:d", options, NULL);

        /* Parsing complete */
        if (c == -1) {
            break;
        }

        switch (c) {
        /* --table_size(-t) option use */
        case 't':
            sscanf(optarg, "%d", &(program_argument.table_size));
            break;
        /* --num_thread(-n) option use */
        case 'n':
            sscanf(optarg, "%d", &(program_argument.num_thread));
            break;
        /* --read_num(-r) option use */
        case 'r':
            sscanf(optarg, "%d", &(program_argument.read_num));
            break;
        /* duration(-d) option use */
        case 'd':
            sscanf(optarg, "%d", &(program_argument.duration));
            break;
        /*  */
        default:
            break;
        }
    }
   
    /* According to specifications */
    if (program_argument.table_size <= 0) {
        puts("'table_size' argument is 0 more than integer, default is 10000");
        flag = true;
    }

    if (program_argument.num_thread <= 0) {
        puts("'num_thread' argument is 0 more than integer "
                "default is number of cores");
        flag = true;
    }

    if (program_argument.read_num <= 0) {
        puts("'read_num' argument is 1-10 integer, default is 10");
        flag = true;
    }
    else if (program_argument.read_num > 10) {
        puts("'read_num' argument is 1-10 integer, default is 10");
        flag = true;
    }

    if (program_argument.duration <= 0) {
        puts("'duration' argument is 0 more than integer, default is 30");
        flag = true;
    }

    return flag;
}

/* Thread routine is transaction */
void*
ThreadFunc(void *arg) {
    /* Passed by main process */
    unsigned long summation;
    unsigned long summation_check;
    /* Summation temp variable */
    unsigned long tmp;
    /* Hash key */
    unsigned long hash_key;
    /* current_time - start_time >= duration is program exit */
    time_t current_time;
    /* Transaciton loop */
    while (true) {
        /* Duration check */
        time(&current_time);
        if (current_time - start_time > program_argument.duration) {
            pthread_exit(0);
        }
        summation = summation_check = 0;
        /* Table A is 0, Table B is 1 */
        int table_id = rand() % 2;
        /* Record 0 is not exist, than + 1 */
        unsigned long random_k = 
            (rand() % (program_argument.table_size - 11) + 1);
        /* Hash alpha */
        unsigned long alpha;
        
        /* transaction initialize */
        struct trx_t trx;
        trx.trx_id = __sync_fetch_and_add(&g_counter, 1);
        trx.trx_state = RUNNING;
       
        /* default table B */
        alpha = 0;
        tmp = g_table_b[random_k].record_value;
        
        /* If table A */
        if (table_id == 0) {
            alpha = program_argument.table_size;
            tmp = g_table_a[random_k].record_value;
        }

        /* READ opeartion */
        for (int i = random_k;
                i < random_k + program_argument.read_num; i++)
        {
            hash_key = random_k + alpha;
            /* wait */
            while (g_lck_hash_table[hash_key].lck_state == EXCLUSIVE)
                puts("READ WAIT");

            g_lck_hash_table[hash_key].lck_state = SHARED;
        }
        
        /* Table A */
        if (table_id == 0) {
            for (int i = random_k; i < random_k + program_argument.read_num;
              i++)
            {
                summation += g_table_a[i].record_value;
             }   
        }
        /* Table B */
        else {
            for (int i = random_k;
                    i < random_k + program_argument.read_num; i++)
            {
                summation += g_table_b[i].record_value;
            }
        }

        /* Free lock */
        for (int i = random_k; i < random_k + program_argument.read_num;
          i++)
        {
            hash_key = random_k + alpha;
            g_lck_hash_table[hash_key].lck_state = NONE;
        }
        __sync_fetch_and_add(&g_read, 1);
       
        /* UPDATE operation */
        for (int i = random_k + program_argument.read_num; 
                i < random_k + 10; i++)
        {
            hash_key = random_k + alpha;
            /* wait */
            while (g_lck_hash_table[hash_key].lck_state != NONE)
                puts("UPDATE WAIT");

            g_lck_hash_table[hash_key].lck_state = EXCLUSIVE;
        }
        
        /* If table A */
        if (table_id == 0) {
            for (int i = random_k + program_argument.read_num;
                    i < random_k + 10; i++)
            {
                g_table_a[i].record_value -= 10;
                g_table_b[i].record_value += 10;
                swap (g_table_a[i], g_table_b[i]);
                g_table_a[i].last_update_trx = trx.trx_id;
                g_table_b[i].last_update_trx = trx.trx_id;
            }
        }
        /* Table B */
        else {
            for (int i = random_k + program_argument.read_num;
                    i < random_k + 10; i++)
            {
                g_table_a[i].record_value += 10;
                g_table_b[i].record_value -= 10;
                swap (g_table_a[i], g_table_b[i]);
                g_table_a[i].last_update_trx = trx.trx_id;
                g_table_b[i].last_update_trx = trx.trx_id;
            }
        }
            
        /* Free lock */
        for (int i = random_k;
                i < random_k + program_argument.read_num; i++)
        {
            hash_key = random_k + alpha;
            g_lck_hash_table[hash_key].lck_state = NONE;
        }
        
        for (int i = random_k;
                i < random_k + program_argument.read_num; i++)
        {
            hash_key = random_k + alpha;
            
            /* If table A */
            if (table_id == 0) {
                summation_check += g_table_a[i].record_value;
            }
            /* Table B */
            else {
                summation_check += g_table_b[i].record_value;
            }
        }
        trx.trx_state = IDLE;
        
        if (summation == summation_check) {
            __sync_fetch_and_add(&g_update, 1);
        }
    }
}

/* Create lock table, all path concerned */
void
CreateLockHashTable(int table_size) {
    /* Hash function is (table_id * 10000) + record_id */
    g_lck_hash_table = 
        (struct lock_hash_table_t *) malloc(
                sizeof(struct lock_hash_table_t) * ((table_size * 2) + 2));
    
    /* Lock state initialize */
    for (int i = 1; i < 2 * table_size + 1; i++) { 
        g_lck_hash_table[i].lck_state = NONE;
    }
    puts("InitLockHashTable!"); 
}

void
CreateRecordTable(int table_size) {
    g_table_a = (struct record_t *) malloc(
            sizeof(struct record_t) * (table_size + 1));
    g_table_b = (struct record_t *) malloc(
            sizeof(struct record_t) * (table_size + 1));
    
    /* Table initialize */
    for (int i = 1; i < table_size + 1; i++) {
        g_table_a[i].record_id = i;
        g_table_b[i].record_id = i;

        g_table_a[i].record_value = 10000 + rand() % 90000;
        g_table_b[i].record_value = rand() % 90000 + 10000;
    
        g_table_a[i].last_update_trx = 0;
        g_table_b[i].last_update_trx = 0;
    }
    puts("InitRecodeTable!");
}

/* Print result */
void
Summary() {
    printf("READ throughput: %lu READS and %lf READS/sec\n",
            g_read, (double) (g_read / program_argument.duration));
    printf("UPDATE throughput: %lu UPDATES and %lf UPDATES/sec\n",
            g_update, (double) (g_update / program_argument.duration));
    printf("Transaction throughput: %lu trx and %lf trx/sec\n",
            g_counter, (double) (g_counter / program_argument.duration));
}

