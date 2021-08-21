#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define true 1
#define false 0

struct sigaction act_new;
struct sigaction act_old;

static void sig_fn(int signo);


int main() {
    act_new.sa_handler = sig_fn;
    sigemptyset(&act_new.sa_mask);

    sigaction(SIGINT, &act_new, &act_old);

    while (true) {
        pause();
    }
}


void sig_fn(int signo) {
    if (signo == SIGINT) {
        fprintf(stdout, "signul number is %d\n", signo);
        fprintf(stdout, "Ctrl-c is pressed, Try Again\n");
    }
}
