#define _XOPEN_SOURCE 600

#ifndef WORKER_T_H
#define WORKER_T_H

#define _GNU_SOURCE

/* To use Linux pthread Library in Benchmark, you have to comment the USE_WORKERS macro */
#define USE_WORKERS 1

/* Targeted latency in milliseconds */
#define TARGET_LATENCY   20  

/* Minimum scheduling granularity in milliseconds */
#define MIN_SCHED_GRN    1

/* Time slice quantum in milliseconds */
#define QUANTUM 10

#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

#include <ucontext.h>
#include <signal.h>
#include <sys/time.h>
#include <string.h>   // For memset
#include <limits.h>   // For INT_MAX

#define READY      0
#define SCHEDULED  1
#define BLOCKED    2 
#define TERMINATED 3

typedef int worker_t;

typedef struct TCB {
  worker_t      thread_id;
  struct TCB*   queue_next;
  struct TCB*   tracker_next;
  unsigned long elapsed_time;
  int           joinable;
  ucontext_t    context;
  int           status;
  int           first_run_time;
  int           creation_time;
  int           completion_time;
  void*         retval;
  void*         stack;
  void*         (*cb)(void*);
  void*         cb_arg;
} tcb;

typedef struct worker_mutex_t {
  int locked;
  tcb* blocked_head;
  tcb* blocked_tail;

} worker_mutex_t;

extern tcb* current;
extern ucontext_t scheduler_context;
extern ucontext_t main_context; 
extern long tot_cntx_switches;  

int worker_yield();
int worker_create(worker_t* thread, pthread_attr_t* attr, void*(*function)(void*), void* arg);
int worker_join(worker_t thread, void* *value_ptr);
void worker_exit(void* value_ptr);

/* Function to print global statistics. Do not modify this function.*/
void print_app_stats(void);

#ifdef USE_WORKERS
#define pthread_t worker_t
#define pthread_mutex_t worker_mutex_t
#define pthread_create(t, a, f, arg) worker_create(t, a, (void*(*)(void*))f, arg)
#define pthread_exit worker_exit
#define pthread_join worker_join
#define pthread_mutex_init worker_mutex_init
#define pthread_mutex_lock worker_mutex_lock
#define pthread_mutex_unlock worker_mutex_unlock
#define pthread_mutex_destroy worker_mutex_destroy
#endif

#endif
