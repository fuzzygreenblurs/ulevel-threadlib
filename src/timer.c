#include "../thread-worker.h"
#include "../include/timer.h"
#include <signal.h>
#include <sys/time.h>
#include <string.h>
#include <ucontext.h>

unsigned long elapsed_quantums = 0;

void timer_isr() {
  elapsed_quantums++;
  fprintf(stderr, "timer tick: %lu\n",elapsed_quantums);
  if(current && current->status == SCHEDULED) {
    swapcontext(&(current->context), &(scheduler_context));
  }
}

void setup_timer() {
  // bind ISR to SIGPROF flag
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa)); 
  sa.sa_handler = timer_isr;
  sigaction(SIGPROF, &sa, NULL); 

  struct itimerval timer;
  
  // reset to countdown value after each interval
  timer.it_interval.tv_sec = 0; 
  timer.it_interval.tv_usec = QUANTUM * 1000;

  // initial timer countdown value 
  timer.it_value.tv_sec  =  0;
  timer.it_value.tv_usec = QUANTUM * 1000;
 
  // start timer
  setitimer(ITIMER_PROF, &timer, NULL);
}
