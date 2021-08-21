/* Simple Multiversion Concurrency Control (MVCC) */
/* 2015-09-21 ~ 2015-10-07 */

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <getopt.h>
#include <time.h>
#include <math.h>
#include <string.h>

/* sync for bakery */
#define SYNC __sync_synchronize()

/* Variable for bakery */
volatile int *flags;
volatile int *labels;

unsigned long global_counter;

/* This variable use for garbage collection */
unsigned long memory_use;

time_t start_time;
time_t duration;

/* update_list[thread_id] = the number of this thread update */
int *update_list;
int num_thread;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/* Using History, Read-view, Actrive-List */
typedef struct _ThreadList {
  int id;
  signed long a;
  signed long b;
  int order;

  struct _ThreadList *next;
} ThreadList;

typedef struct _Thread {
  int id;
  signed long a;
  signed long b;
  int order;

  ThreadList *history;
  ThreadList *his_cur;

  ThreadList *read_view;
  ThreadList *read_cur;
} Thread;

ThreadList *active_list;
ThreadList *active_cur;
/* 리스트의 초기화는 더미노드를 만드는 것입니다. (order와 id값이 -1) */

/* 쓰레드 초기화 함수, 변수 a, b를 랜덤으로 초기화하고 생성된 순으로 쓰레드 id를 부여하고, history도 초기화합니다. */
void ThreadInit(Thread *thread, int id);
/* 쓰레드 업데이트 함수, 변수 a, b를 새 랜덤 값으로 업데이트 하고, 업데이트 한 횟수를 update_list[id]에 표기하고 history를 추가합니다. */
void ThreadUpdate(Thread *thread);
/* 업데이트가 끝난 후 쓰레드 id를 active_list에서 제거합니다. */
void RemoveActive(int id);
/* 업데이트에 시작하면, 업데이트 중이라고 active_list에 삽입합니다. */
void InsertActive(Thread *thread);
/* history에 해당 쓰레드를 추가합니다. */
void InsertHistory(Thread *thread);
/* active_list를 초기화합니다. */
void ActiveListInit();
/* ReadView를 초기화합니다. */
void ReadViewInit(Thread *thread);
/* Snap-shot 찰칵 */
void ReadView(Thread *thread);
/* duration만큼의 시간(초)가 지나면 Throughput과, fairness 계산, 출력하는 함수입니다. */
void Calc(time_t duration);
/* pthread_cleanup 함수입니다. */
void CleanUpHandler(void *arg);
/* 업데이트가 완료되면 해당 쓰레드의 ReadView를 삭제합니다. */
void RemoveReadView(Thread *thread);
/* history를 초기화합니다. */
void HistoryInit(Thread *thread);
/* 더 이상 필요없는  history를 쓰레드에서 제거합니다. */
void RemoveHistory(Thread *thread);

/* Bakery Lock & Unlock */
void BakeryLock(int id);
void BakeryUnlock(int id);

/* 쓰레드 실행 함수입니다. */
void* ThreadFunc(void *arg);
/* 미구현 */
void* GarbageCollection(void *arg);

/*쓰레드 하나를 추가하여 duration을 계산하는 쓰레드입니다. */
/*void* TimeThreadFunc (void *arg);*/

/*register num_thread;*/
/*time_t duration;*/

int main(int argc, char *argv[]) {
  int opt;
  /*time_t duration = 0;*/
  static int verify = 0;

  void *create_arg;
  void *join_arg;

  int i, j;

  pthread_t *thread;
  /*pthread_t time_thread;*/

  global_counter = 0;
  duration = 0;
  num_thread = 1;
  memory_use = 0;
  time (&start_time);

  while (1) {
    /* 인자 구분 */
    static struct option options[] = {
      {"num_thread", required_argument, NULL, 'n'},
      {"duration", required_argument, NULL, 'd'},
      {"verify", no_argument, &verify, '1'},
      {0, 0, 0, 0}
    };

    opt = getopt_long (argc, argv, "n:d:v", options, NULL);

    if (opt == -1) break;

    switch (opt) {
      /* case 0 is getopt_long function */
      case 0:      
        /* --verify */
        break;
      /* getopt fuction */
      case 'n':
        if ((num_thread = atoi (optarg)) <= 0) {
          puts ("num_thread should be positive integer, default value is '1'");
          return 0;
        }
        break;

      case 'd':
        if ((duration = atoi (optarg)) <= 0) {
          puts ("duration should be necessary option and positive integer");
          return 0;
        }
        break;

      case 'v':
        verify = 1;
        break;

      case '?':
        /* No argument */
        break;

      default:
        puts ("Only options: --num_thread (or -n), --duration (or -d), --verfiy (or -v)");
        abort ();
        break;
    }
  }

  /* 변수들과 리스트를 초기화하고, 각각 필요한 만큼 메모리를 할당합니다. */
  /*pthread_create (&time_thread, NULL, TimeThreadFunc, (void *)duration);*/
  update_list = (int *) malloc (sizeof(int) * num_thread);
  memory_use += (sizeof(int) * num_thread);
  memset ((void *)update_list, 0, sizeof(int) * num_thread);

  flags = (int *) malloc (sizeof(int) * num_thread);
  memory_use += (sizeof(int) * num_thread);
  labels = (int *) malloc (sizeof(int) * num_thread);
  memory_use += (sizeof(int) *num_thread);

  memset((void *)flags, 0, sizeof(int) * num_thread);
  memset((void *)labels, 0, sizeof(int) * num_thread);

  thread = (pthread_t *) malloc(sizeof(pthread_t) * num_thread); memory_use += (sizeof(pthread_t) * num_thread);
  ActiveListInit();

  for (i = 0; i < num_thread; i++)
    pthread_create(&thread[i], NULL, ThreadFunc, (void *)i);

  for (i = 0; i < num_thread; i++)
    pthread_join(thread[i], (void **) &join_arg);

  Calc(duration);
  free(thread);
  printf("Memory use: %lu\n", memory_use);

  return 0;
}

void BakeryLock (int id) {
  flags[id] = 1;
  SYNC;

  int max = 0;
  int label;
  int i, j;

  for (i = 0; i < num_thread; i++) {
    label = labels[i];

    if (label > max)
      max = label;
  }

  labels[id] = max + 1;
  SYNC;
  flags[id] = 0;
  SYNC;

  for (j = 0; j < num_thread; j++) {
    while (flags[j]);
    SYNC;
    while ((labels[j] != 0)
      && (labels[j] < (labels[id] || ((labels[j] == labels[id]) && (j < id)))));
  }
}

void BakeryUnlock (int id) {
  SYNC;
  labels[id] = 0;
}

/*void time_handler (void *arg) {

  Calc ();
}*/

void* GarbageCollection (void *arg) {

}

void CleanUpHandler (void *arg) {
  Thread *thread = (Thread *) arg;
  RemoveHistory (thread);
}

void* ThreadFunc (void *arg) {
  Thread thread;
  int id = (int) arg;
  time_t now;

  /*pthread_cleanup_push (cleanup_handler, (void *)pthread_self ());*/
  ThreadInit (&thread, id);
  pthread_cleanup_push (CleanUpHandler, (void *) &thread);

  while (1) {
    time (&now);
    /*if ((double)(clock () - start_time) >= (double)(duration * CLOCKS_PER_SEC)){*/
    if (now - start_time >= duration) {
      /* 인자로 받은 시간 만큼의 초가 지나면 쓰레드 종료 요청을 보내고, 핸들러가 처리합니다. */
      pthread_cancel (pthread_self());
      break;
    }

    ThreadUpdate(&thread);
    printf ("Thread[%d], update\n", id);
  }

  pthread_cleanup_pop(0);
}

void ThreadInit (Thread *thread, int id) {
  (*thread).id = id;
  srand(time (NULL));
  (*thread).a = rand() >> rand();
  (*thread).b = rand() % rand();
  (*thread).order = 0;

  HistoryInit (thread);
  /*printf ("Init Thread[%d]\n", id);*/
}

void HistoryInit (Thread *thread) {
  (*thread).history = (ThreadList *) malloc(sizeof(ThreadList));
  memory_use += sizeof(ThreadList);

  (*thread).history->a = (*thread).a;
  (*thread).history->b = (*thread).b;
  (*thread).history->order = 0;
  (*thread).history->next = NULL;
  (*thread).his_cur = NULL;
}

void ThreadUpdate (Thread *thread) {
  /* atomic한 계산을 위해 gcc에서 지원하는 FAD를 사용했습니다. */
  __sync_fetch_and_add(&(*thread).order, 1);

  /*BakeryLock ((*thread).id);*/
  pthread_mutex_lock(&mutex);
  InsertActive(thread);
  /*ReadView(thread);*/
  pthread_mutex_unlock(&mutex);
  /*BakeryUnlock((*thread).id);*/

  unsigned long sum = (*thread).a + (*thread).b;
  (*thread).a = rand() % rand();
  (*thread).b = sum - (*thread).a;
  
  ReadView(thread);
  InsertHistory(thread);

  /*BakeryLock ((*thread).id);*/
  pthread_mutex_lock(&mutex);
  RemoveActive((*thread).id);
  pthread_mutex_unlock(&mutex);
  /*BakeryUnlock ((*thread).id);*/
  
  __sync_fetch_and_add(&update_list[(*thread).id], 1);
  /*pthread_mutex_lock (&mutex);*/
  /*RemoveReadView (thread);*/
  /*pthread_mutex_unlock (&mutex);*/
}

/*void* TimeThreadFunc (void *arg) {

  time_t duration = (time_t)arg;
  
  while (1) {
    
  if ((double)(clock () - start_time)  >= (double)(duration * CLOCKS_PER_SEC)) {
    
      Calc (duration);
      exit (1);
    }
  }
}*/

void RemoveActive(int id) {
  ThreadList *cur = active_list;
  ThreadList *tmp;

  while (cur->next != NULL) {
    tmp = cur;
    cur = cur->next;

    if (cur->id == id) {
      tmp->next = cur->next;
      free(cur);
    }
  }
}

void InsertActive(Thread *thread) {
  active_cur = (ThreadList *) malloc(sizeof(ThreadList));
  memory_use += sizeof(ThreadList);

  active_cur->id = (*thread).id;
  active_cur->order = (*thread).order;
  active_cur->next = NULL;

  ThreadList *cur = active_list;

  while (cur->next != NULL) {
    cur = cur->next;
  }

  cur->next = active_cur;

  ThreadList *tmp = active_list;

  while (tmp->next != NULL){
    tmp = tmp->next;
    printf("Active List %d\n", tmp->id);
  }
}

void InsertHistory(Thread *thread) {

  (*thread).his_cur = (ThreadList *) malloc(sizeof(ThreadList));
  memory_use += sizeof(ThreadList);

  (*thread).his_cur->id = (*thread).id;
  (*thread).his_cur->order = (*thread).order;
  (*thread).his_cur->a = (*thread).a;
  (*thread).his_cur->b = (*thread).b;
  (*thread).his_cur->next = NULL;

  ThreadList *cur = (*thread).history;

  while (cur->next != NULL) {
    cur = cur->next;
  }

  cur->next = (*thread).his_cur;
}

void ActiveListInit() {

  active_list = (ThreadList *) malloc (sizeof(ThreadList));
  memory_use += sizeof(ThreadList);

  active_list->next = NULL;
  active_list->id = -1;
}

void ReadView(Thread *thread) {
  ReadViewInit (thread);

  ThreadList *cur = active_list;
  ThreadList *rv = (*thread).read_view;
  /*int pick = rand () % num_thread;*/

  /* Active list copy to thread's read-view */
  while (cur != NULL) {
    if (cur->order < (*thread).order) {
      (*thread).read_cur = (ThreadList *) malloc(sizeof(ThreadList));
      memory_use += sizeof(ThreadList);
      (*thread).read_cur->id = cur->id;
      (*thread).read_cur->order = cur->order;
      (*thread).read_cur->next = cur->next;
      /*printf ("before %p %p\n", rv, rv->next);*/
      rv->next = (*thread).read_cur;
      /*printf ("after %p %p\n", rv, rv->next);*/
      rv = rv->next;
    }
    
    cur = cur->next;
  }
}

void ReadViewInit(Thread *thread) {
  (*thread).read_view = (ThreadList *) malloc (sizeof(ThreadList));
  memory_use += sizeof(ThreadList);
  (*thread).read_view->order = -1;
  (*thread).read_view->next = NULL;
  (*thread).read_cur = NULL;
}

void Calc(time_t duration) {
  long double sec = (long double)duration;
  long double sum = 0;
  int i;

  long double calc_a;
  long double calc_b = 0;

  for (i = 0; i < num_thread; i++) {
    sum += update_list[i];
    calc_b += pow(update_list[i], 2);
  }

  calc_a = pow(sum, 2);

  printf("throughput = %Lf / %Lf = %Lf\n", sum, sec, sum / sec);
  printf("fairness = %Lf\n", calc_a / (calc_b * num_thread));
}

void RemoveReadView (Thread *thread) {
  ThreadList *cur = (*thread).read_view;

  if (cur->next == NULL) {
    return ;
  }

  while (cur->next != NULL) {
    (*thread).read_cur = cur;
    cur = cur->next;
    free ((*thread).read_cur);
  }
}

void RemoveHistory (Thread *thread) {
  ThreadList *cur = (*thread).history;

  while (cur->next != NULL) {
    (*thread).his_cur = cur;
    cur = cur->next;
    free (&(*thread).his_cur);
  }
}
