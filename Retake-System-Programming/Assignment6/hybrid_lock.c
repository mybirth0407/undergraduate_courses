#include "hybrid_lock.h"
#include <pthread.h>
#include <sys/time.h>

int
hybrid_lock_init(hybrid_lock *myhybrid)
{
    // init failed
    if (pthread_spin_init(&myhybrid->s_lock, 0) != 0)
    {
        fprintf(stderr, "spin lock init failed");
        return -1;
    }
    if (pthread_mutex_init(&myhybrid->m_lock, 0) != 0)
    {
        fprintf(stderr, "mutex lock init failed!\n");
        return -1;
    }
    // init success!
    return 0;
}


int
hybrid_lock_destroy(hybrid_lock *myhybrid)
{
    // destroy failed
    if (pthread_spin_destroy(&myhybrid->s_lock) != 0)
    {
        fprintf(stderr, "spin lock destroy failed!\n");
        return -1;
    }
    if (pthread_mutex_destroy(&myhybrid->m_lock) != 0)
    {
        fprintf(stderr, "mutex lock destroy failed!\n");
        return -1;
    }
    // destroy success
    return 0;
}


void
hybrid_lock_lock(hybrid_lock *myhybrid)
{
    struct timeval start, end;
    int judge;
    double timecost;
    // double과 byte수 맞추기 위해서 long 선언
    long count, i;
    
    /**
     * 평균 시간을 구하기 위해 1000회(평균을 구하기 적당한 시간) 시도한다.
     */
    gettimeofday(&start, NULL);

    for (i = 0; i < 1000; ++i) {
        if (pthread_spin_trylock(&myhybrid->s_lock) == 0)
        {
            judge = pthread_mutex_trylock(&myhybrid->m_lock);

            if (judge != 0)
            {
                // spin lock 획득 성공, mutex lock 획득 실패시 spin lock 해제
                pthread_spin_unlock(&myhybrid->s_lock);
            } else
            {
                // spin lock, mutex lock 둘다 획득 성공
                return;
            }
        }
    }

    gettimeofday(&end, NULL);
    // 1000회 반복한 평균 시간
    timecost = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    // 1초동안 반복하는 횟수
    count = (long) (1000.0 / timecost);

    // 1 loop is 1 second, for real world time
    for (i = 0; i < count; ++i)
    {
        if (pthread_spin_trylock(&myhybrid->s_lock) == 0)
        {
            judge = pthread_mutex_trylock(&myhybrid->m_lock);

            if (judge != 0)
            {
                // spin lock 획득 성공, mutex lock 획득 실패시 spin lock 해제
                pthread_spin_unlock(&myhybrid->s_lock);
            } else
            {
                // spin lock, mutex lock 둘다 획득 성공
                return;
            }
        }
    }

    // spin lock, mutex lock 모두 획득하지 못한 상황
    // spin lock을 먼저 시도할 경우
    // finite한 시간 내에 lock을 획득하지 못할 수도 있다.
    // mutex lock부터 lock을 시도해야만 효율성을 높일 수 있다.
    pthread_mutex_lock(&myhybrid->m_lock);
    pthread_spin_lock(&myhybrid->s_lock);
}


void
hybrid_lock_unlock(hybrid_lock *myhybrid)
{
    // 해당 과정은 반드시 성공한다.
    pthread_mutex_unlock(&myhybrid->m_lock);
    pthread_spin_unlock(&myhybrid->s_lock);
}