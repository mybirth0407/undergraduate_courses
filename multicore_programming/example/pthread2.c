#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void *t_func (void *data) {
  int i = 0;
  int thread_num = *((int *)data);

  while (1) {
    printf ("Thread %d, i = %d\n", thread_num, i);

    if ((i > 5) && (thread_num == 1)) {
      pthread_exit (0);
    }

    sleep (1);
    i++;
  }
}

int main () {
  pthread_t _thread[2];

  int proc_a = 1;
  int proc_b = 2;

  int a_thread_return, b_thread_return;

  pthread_create (&_thread[0], NULL, t_func, (void *)&proc_a);
  pthread_create (&_thread[1], NULL, t_func, (void *)&proc_b);

  pthread_join (_thread[1], (void **)&b_thread_return);

  return 0;
}

