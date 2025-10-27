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
  worker_t thread_id;
  struct TCB* queue_next;
  int         elapsed_time;
} tcb;


//typedef struct TCB {
//  worker_t    thread_id;
//  int         status;
//  ucontext_t  context;
//  void*       stack;
//  struct TCB* runqueue_next; 
//  struct TCB* tracked_next;
//  int         joinable;
//  int         elapsed_time;
//  void*       retval;
//
//  void*       (*cb)(void*);
//  void*       cb_arg;
//
//  int         creation_time;
//  int         first_run_time;
//  int         completion_time;
//} tcb; 

#endif
