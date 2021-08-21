#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct hybrid_lock {
    pthread_spinlock_t s_lock;
    pthread_mutex_t m_lock;
} hybrid_lock; 

/**
  * hybrid lock을 초기화한다.
  * hybrid lock structure 내부의 spin lock과 mutex lock을 초기화한다.
  * 초기화 성공시 0을 반환하고,
  * 두 lock 중 하나의 lock이라도 초기화 실패시 -1을 반환한다.
  */
int hybrid_lock_init(hybrid_lock *myhybrid);

/**
  * hybrid lock을 소멸시킨다.
  * hybrid lock structure 내부의 spin lock과 mutex lock을 소멸시킨다.
  * 초기화 성공시 0을 반환하고,
  * 두 lock 중 하나의 lock이라도 초기화 실패시 -1을 반환한다.
  */
int hybrid_lock_destroy(hybrid_lock *myhybrid);

/**
  * hybrid lock를 획득한다.
  * thread가 1초간 spin하며 turn을 대기하고,
  * 1초 이후에는 mutex를 사용하여 대기한다.
  */
void hybrid_lock_lock(hybrid_lock *myhybrid);

/**
  * hybrid lock를 해제한다.
  * spin lock과 mutex lock을 모두 unlock 한다.
  */
void hybrid_lock_unlock (hybrid_lock *myhybrid);
