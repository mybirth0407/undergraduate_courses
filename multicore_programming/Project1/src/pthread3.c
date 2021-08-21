#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

int *int_data;

void cleanup_handler (void *arg) {
  printf ("Cleanup handler is called.\n");
  free (int_data);
  printf ("free ()\n");
}

void *t_func (void *data) {
  int i = 0;
  int tmp = 0;
  int thread_num = *((int *)data);

  int_data = (int *)malloc (sizeof (int) * 100);

  pthread_cleanup_push (cleanup_handler, (void *)&tmp);

  while (1) {
    if (i > 5) {
      pthread_exit (0);
    }

    printf ("i = %d\n", i);
    i++;
    sleep (1);
  }

  pthread_cleanup_pop (0);
}

int main () {
  pthread_t _thread;

  int proc_a, a_thread_return;

  pthread_create (&_thread, NULL, t_func, (void *)&proc_a);
  printf ("pthread_create ()\n");

  pthread_join (_thread, (void **)&a_thread_return);
  printf ("pthread_join ()\n");

  return 0;
}
