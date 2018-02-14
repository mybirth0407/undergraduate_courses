#include <stdio.h>
#include <inttypes.h>
#include <pthread.h>

#define NUM_PRODUCER                32
#define NUM_CONSUMER                NUM_PRODUCER
#define NUM_THREADS                 (NUM_PRODUCER + NUM_CONSUMER)
#define NUM_ENQUEUE_PER_PRODUCER    1000000
#define NUM_DEQUEUE_PER_CONSUMER    NUM_ENQUEUE_PER_PRODUCER

bool flag_verification[NUM_PRODUCER * NUM_ENQUEUE_PER_PRODUCER];
void enqueue(int key);
int dequeue();

// ------------- Bounded lock-free queue -------------

#define QUEUE_SIZE      1024

struct QueueNode {
    int key;
    uint64_t flag;
};

QueueNode queue[QUEUE_SIZE];
uint64_t front;
uint64_t rear;

pthread_mutex_t mutex_for_queue = PTHREAD_MUTEX_INITIALIZER;

void init_queue(void) {
    front = 0;
    rear = 0;
    for (int i = 0; i < QUEUE_SIZE; i++) {
        queue[i].key = 0;
        queue[i].flag = 0;
    }
}

void enqueue(int key) {
    uint64_t seq = __sync_fetch_and_add(&rear, 1);
    int slot_idx = seq % QUEUE_SIZE;
    uint64_t round = seq / QUEUE_SIZE;

    while (1) {
        uint64_t flag = queue[slot_idx].flag;
        if (flag % 2 == 1) {
            // Queue full
            pthread_yield();
        } else {
            if (flag / 2 == round) {
                // for fairness
                queue[slot_idx].key = key;
                __sync_synchronize();
                queue[slot_idx].flag++;
                break;
            } else {
                pthread_yield();
            }
        }
    }
}

int dequeue(void) {
    uint64_t seq = __sync_fetch_and_add(&front, 1);
    int slot_idx = seq % QUEUE_SIZE;
    uint64_t round = seq / QUEUE_SIZE;
    int ret_key;

    while (1) {
        uint64_t flag = queue[slot_idx].flag;
        if (flag % 2 == 0) {
            // Queue empty
            pthread_yield();
        } else {
            if (flag / 2 == round) {
                // for fairness
                ret_key = queue[slot_idx].key;
                __sync_synchronize();
                queue[slot_idx].flag++;
                break;
            } else {
                pthread_yield();
            }
        }
    }

    return ret_key;
}
// ------------------------------------------------

void* ProducerFunc(void* arg) {
    long tid = (long)arg;

    int key_enqueue = NUM_ENQUEUE_PER_PRODUCER * tid;
    for (int i = 0; i < NUM_ENQUEUE_PER_PRODUCER; i++) {
        enqueue(key_enqueue);
        key_enqueue++;
    }

    return NULL;
}

void* ConsumerFunc(void* arg) {
    for (int i = 0; i < NUM_DEQUEUE_PER_CONSUMER; i++) {
        int key_dequeue = dequeue();
        flag_verification[key_dequeue] = true;
    }

    return NULL;
}

int main(void) {
    pthread_t threads[NUM_THREADS];

    init_queue();

    for (int i = 0; i < NUM_THREADS; i++) {
        if (i < NUM_PRODUCER) {
            pthread_create(&threads[i], 0, ProducerFunc, (void**)i);
        } else {
            pthread_create(&threads[i], 0, ConsumerFunc, NULL);
        }
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // verify
    for (int i = 0; i < NUM_PRODUCER * NUM_ENQUEUE_PER_PRODUCER; i++) {
        if (flag_verification[i] == false) {
            printf("INCORRECT!\n");
            return 0;
        }
    }
    printf("CORRECT!\n");

    return 0;
}

