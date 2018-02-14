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
    struct QueueNode *next;
};

QueueNode *front;
QueueNode *rear;

pthread_mutex_t mutex_for_queue = PTHREAD_MUTEX_INITIALIZER;

void init_queue(void) {
    QueueNode *sentinal = (QueueNode *) malloc(sizeof(QueueNode));
    sentinal->next = NULL;
    front->next = sentinal;
    rear->next = sentinal;
}

void enqueue(int key) {
    struct QueueNode *e_node = new QueueNode();
    e_node->key = key;
    e_node->next = NULL;

    // simple version
    while (!__sync_bool_compare_and_swap(&rear->next, NULL, e_node)) {
        pthread_yield();
    }
    // 성공한 쓰레드만이 가능하다.
    // 위의 루프를 탈출했다는 것이 cas를 성공했다는 것이기 때문
    rear = e_node;

    // book version
    // scalable
    // QueueNode *rear_copied;
    
    // while (true) {
    //     rear_copied = rear;
    //     // 성공시 old 값을 리턴
    //     QueueNode *ret_node = __sync_val_compare_and_swap(
    //         &rear_copied->next, NULL, e_node);
        
    //     if (!ret_node) {
    //         break;
    //     }
    //     else {
    //         __sync_bool_compare_and_swap(&rear, rear_copied, ret_node);
    //     }
        
    // }
    // // 대입으로 하면 안됨
    // if (rear == rear_copied)
    //     __sync_bool_compare_and_swap(&rear, rear_copied, e_node);
}

int dequeue(void) {
    QueueNode *sentinal;
    int ret_key;
    
    while (true) {
        sentinal = front;
        
        if (sentinal->next != NULL) {
            if (__sync_bool_compare_and_swap(&front, sentinal, sentinal->next)) {
                ret_key = sentinal->next->key;
                break;
            }
        }
        else {
            // queue is empty
            // 책에서는 non-blocking 사용
            pthread_yield(); // blocking
        }
    }
    
    // 동일한 sential을 읽은 다른 thread와의 문제가 생길 수 있음
    // 막 쓰면 안됨
    // free(sentinal);

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

