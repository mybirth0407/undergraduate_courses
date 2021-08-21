#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>

void *t_func (void *data) {
  pid_t pid;
  pthread_t tid;

  int thread_num = *((int *)data);

  pid = getpid ();
  tid = pthread_self ();

  printf ("Thread %d's PID is %d\n", thread_num, pid);
  printf ("Thread %d's PID is thread%d is %d\n", thread_num, thread_num, tid);

  return (void *)(thread_num + 10);
}

int main () {
  pthread_t _thread[2];

  int proc_a = 1;
  int proc_b = 2;

  int a_thread_return, b_thread_return;

  printf ("Parent PID: %d\n", getpid ());
  
  pthread_create (&_thread[0], NULL, t_func, (void *)&proc_a);
  pthread_create (&_thread[1], NULL, t_func, (void *)&proc_b);

  printf ("Thread 1's TID in parent is %d\n", _thread[0]);
  printf ("Thread 2's TID in parent is %d\n", _thread[1]);

  pthread_join (_thread[0], (void **)&a_thread_return);
  pthread_join (_thread[1], (void **)&b_thread_return);

  return 0;
}
