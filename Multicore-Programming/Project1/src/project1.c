/* Multicore Programming Project1 */
/* This program is fast prime number by divide sqrt (n) */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include <getopt.h>
#include <math.h>
#include <time.h>

/* This algorithm is divide sqrt (n) */
void* fast_prime_search(void *arg);
/* Input 'n', If 'n' is prime number, return 1, if not return 0 */
int IsPrime(int n);

/* Flag set by --verbose || -v */
char *arr;
static int verbose_flag;
static int num;
static int num_of_prime;

typedef struct _stats {
  unsigned long s;
  unsigned long e;
} stats;

int main(int argc, char *argv[]) {
  unsigned num_thread = 1;
  unsigned long start = 1;
  unsigned long end = 1;
  int i;

  /* Function "getopt_long" for parsing */
  int opt;
  int option_index;

  pthread_t *_thread;
  stats *stat;

  while (1) {
    static struct option options[] = {
      /* {"option name", whether argument, value address to option set, value for set} */
      {"num_thread", required_argument, 0, 'n'},
      {"start", required_argument, 0, 's'},
      {"end", required_argument, 0, 'e'},
      {"verbose", no_argument, 0, 'v'},
      /* Must finished struct option {0, 0, 0, 0} */
      {0, 0, 0, 0}
    };

    /* Argument option parsing */
    opt = getopt_long (argc, argv, "n:s:e:v", options, &option_index);

    /* if option parsing finished, then break */
    if (opt == -1)
      break;

    switch (opt) {
      /* Use getopt_long (long argument, ex) --num_thread, --start, --end, --verbose)*/
    case 0:
      if (options[option_index].flag != 0) {
        break;
      }
      
      printf ("option %s", options[option_index].name);
      
      if (optarg) {
        printf (" with arg %s", optarg);
      }
      
      printf ("\n");
      break;

      /* Use getopt case n, s, e, v */
    case 'n':
      if ((num_thread = atoi (optarg)) <= 0) {
        puts ("Please provide correct argument: \"num_thread\" should be positive integer");
        return 0;
      }
      break;

    case 's':
      /* "start" argument is less than or equal to 0 or not integer */
      if ((start = atoi (optarg)) <= 0) {
        puts ("Please provide correct argument: \"start\" should be positive integer and less than \"end\" argument");  
        return 0;
      }  
      break;

    case 'e':
      if ((end = atoi (optarg)) <= 1) {
        puts ("Please provide correct argument: \"end\" should be positive integer and greater than \"start\" argument\nIf --start or -s option is not setting, \"start\" is default 1");
        return 0;
      }
      break;

    case 'v':
      verbose_flag = 1;
      break;

    case '?':
      /* No argument */
      break;

    default:

      puts ("options only: --num_thread (-n), --start (-s), --end (-e), --verbose (-v)");
      abort ();
    }
  }
  
  if (start >= end) {
    puts ("\"start\" argument should be less than \"end\" argument, \"start\" default is 1");
    return 0;
  }

  /* Dynamic allocate */
  _thread = (pthread_t *)malloc (sizeof (pthread_t) * num_thread);
  stat = (stats *)malloc (sizeof (stats) * num_thread);

  /* "arr" is char array, index is number, element is like bool */
  arr = (char *)calloc (end + 1, sizeof (char));

  stat[0].s = start;
  stat[0].e = ((end - start) / num_thread) + start;

  for (i = 1; i < num_thread; i++) {
    stat[i].s = (i * (end - start) / num_thread) + start + 1;
    stat[i].e = (((i + 1) * (end - start)) / num_thread) + start;
  }

  num_of_prime = 0;

  /* "stat" is structure, include section for thread[i] */
  for (i = 0; i < num_thread; i++) {
    pthread_create (&_thread[i], NULL, fast_prime_search, (void *)&stat[i]);
  }

  for (i = 0; i < num_thread; i++) {
    pthread_join (_thread[i], NULL);
  }

  /* 1 is not Prime Number */
  if (arr[1] = 1) {
    arr[1] = 0;
  }

  /* "verbose_flag" is set 1, Use --verbose or -v */
  if (verbose_flag) {
    puts ("Verbose (--verbose || -v) option using"); 
  
    for (i = start; i <= end; i++) {
      if (arr[i] == 1) {
        printf ("%d\n", i);
        num_of_prime++;
      }
    }
  }
  /* "verbose_flag" is set 0, Not use vebose option */
  else {
    for (i = start; i <= end; i++)
      if (arr[i] == 1) {
        num_of_prime++;
      }
  }

  printf ("The number of prime number is %d\n", num_of_prime);
  return 0;
}

void* fast_prime_search(void *arg) {
  stats *stat = (stats *)arg;
  unsigned long start = stat->s;
  unsigned long end = stat->e;
  int i;

  for (i = start; i <= end; i++) {
    /* Repeition check remove */
    if (arr[i] == 1) {
      continue;
    }
  
    if (IsPrime (i)) {
      arr[i] = 1;
    }
  }
  /* Number "num" is prime number */
  arr[num] = 0;

  return (void *)0;
}

int IsPrime(int n) {
  int i;

  if (n % 2 == 0) {
    return 0;
  }

  /* Divide sqrt (n) */
  for (i = 2; ((2 * i) - 1) <= sqrt(n); i++)
    /* 'n' is not prime number */
    if (n % ((2 * i) - 1) == 0)
      return 0;
  /* 'n' is prime number */
  return 1;
}

