#include "../thread-worker.h"
#include "../include/timer.h"
#include "../include/queue.h"
#include "../include/tracker.h"
#include "../include/scheduler.h"

// global variables
tcb* current = NULL;
ucontext_t scheduler_context;
ucontext_t main_context;
long tot_cntx_switches = 0;

double avg_turn_time = 0; 
double avg_resp_time = 0;
static long accum_turn_time = 0;
static long accum_resp_time = 0;

static int scheduler_initialized = 0;
static worker_t latest_assigned_id = 0;
static long completed_threads = 0;


void worker_exit(void* value_ptr) {
  current->retval = value_ptr;
  free(current->stack);            
  current->status = TERMINATED;
  current->completion_time = elapsed_quantums;

  accum_turn_time += current->completion_time - current->creation_time; 
  accum_resp_time += current->first_run_time  - current->creation_time;
  completed_threads++;

  avg_turn_time = (double)(accum_turn_time/completed_threads) * QUANTUM;
  avg_resp_time = (double)(accum_resp_time/completed_threads) * QUANTUM;

  setcontext(&scheduler_context);
}

static void store_cb_retval() {
  void* ret = current->cb(current->cb_arg);
  worker_exit(ret);
}

/* return control to the OS voluntarily */
int worker_yield() {
  
  // - change worker state from RUNNING to READY
  // - save context of this thread to its TCB 
  // - switch from thread context to scheduler context

  if(current == NULL) {
    swapcontext(&main_context, &scheduler_context);
    return 0;
  }

  current->status = READY;
  enqueue(current);
  swapcontext(&(current->context), &scheduler_context);
  
  return 0;
};

// force the caller to wait until a specific thread terminates
int worker_join(worker_t thread, void **value_ptr) {
  tcb* prev = NULL;
  tcb* candidate = find_thread_tracker(thread, &prev);
  if(candidate == NULL) return -1;

  while(candidate->status != TERMINATED) {
    worker_yield();
  }

  if(value_ptr != NULL) {
    *value_ptr = candidate->retval;
  }

  untrack(candidate, prev);
  return 0;
};

int worker_create(
  worker_t* thread, 
  pthread_attr_t* attr, 
  void*(*function)(void*), 
  void* arg) {
  // - allocate space of stack for this thread to run
  // after everything is set, push this thread into run queue and 
  // - make it ready for the execution.

  // TODO: break up into individual functions. this is bloated.
  if(!scheduler_initialized) {
    scheduler_initialized = 1;

    getcontext(&scheduler_context);
    getcontext(&main_context);
    scheduler_context.uc_link = NULL;
    scheduler_context.uc_stack.ss_sp = malloc(SIGSTKSZ);
    scheduler_context.uc_stack.ss_size = SIGSTKSZ;
    scheduler_context.uc_stack.ss_flags = 0;

    makecontext(&scheduler_context, schedule, 0);

    setup_timer();
  }

  // - create Thread Control Block (TCB)
  // TODO: does creating the new thread need to be atomic? 
  tcb* worker_tcb = (tcb*)malloc(sizeof(tcb)); 
  if(!worker_tcb) return -1;                     // TODO: assert/raise readable error?
  worker_tcb->thread_id = latest_assigned_id++;
  worker_tcb->elapsed_time = 0;
  *thread = worker_tcb->thread_id; 

  worker_tcb->creation_time = elapsed_quantums;
  worker_tcb->first_run_time = -1;
  worker_tcb->cb = function;
  worker_tcb->cb_arg = arg;

  // create and initialize the context of this worker thread      
  // getcontext initializes internal fields of the gien ucontex_t struct
  // it captures the current CPU state as a starting template
  // setup proper signal masks, flags and architectture specific data
  if(getcontext(&(worker_tcb->context)) < 0) {
    free(worker_tcb); 
    return -1;
  } 

  // worker_tcb->stack is of type void*
  worker_tcb->stack = malloc(SIGSTKSZ);
  if(!worker_tcb->stack) {
    free(worker_tcb);
    return -1;
  }

  // STEP 2 TODO: notes on ss_flags, uc_link?
  worker_tcb->context.uc_link = NULL;
  worker_tcb->context.uc_stack.ss_sp = worker_tcb->stack;
  worker_tcb->context.uc_stack.ss_size = SIGSTKSZ;
  worker_tcb->context.uc_stack.ss_flags = 0;

  // TODO: verify if this is the right way to cast this function pointer
  makecontext(&(worker_tcb->context), 
              (void(*)(void))store_cb_retval,
              0);
              
  track(worker_tcb);
  enqueue(worker_tcb);
  worker_tcb->status = READY;
  return 0;
}

int worker_mutex_init(worker_mutex_t *mutex, 
        const pthread_mutexattr_t *mutexattr) {
	//- initialize data structures for this mutex
  mutex->locked = 0;
  mutex->blocked_head = NULL;
  mutex->blocked_tail = NULL;
	return 0;
};

int worker_mutex_lock(worker_mutex_t *mutex) {
  // -:149
  // :use the built-in test-and-set atomic function to test the mutex
  // - if the mutex is ahow me aquired successfully, enter the critical section
  // - if acquiring mutex fails, push current thread into block list and
  // context switch to the scheduler thread

  if(__sync_lock_test_and_set(&mutex->locked, 1)) {
    current->status = BLOCKED;

    if(mutex->blocked_tail == NULL) {
      mutex->blocked_head = current;
    } else {
      mutex->blocked_tail->queue_next = current;
    }
    mutex->blocked_tail = current;
    current->queue_next = NULL;
    
    swapcontext(&(current->context), &scheduler_context);
  } 
  
  return 0;
};

int worker_mutex_unlock(worker_mutex_t *mutex) {
	// - release mutex and make it available again. 
	// - put threads in block list to run queue 
	// so that they could coimpete for mutex later.

  if(mutex->blocked_head != NULL) {
    tcb* unblocked = mutex->blocked_head;
    mutex->blocked_head = unblocked->queue_next;
    unblocked->status = READY;
    enqueue(unblocked);

    if(mutex->blocked_head == NULL) {
      mutex->blocked_tail = NULL;
    }
  }

  mutex->locked = 0;

  return 0;
};

int worker_mutex_destroy(worker_mutex_t *mutex) {
  // mutexes are assumed to be stack allocated and will be
  // de-allocated at the end of the caller function
  if (mutex->blocked_head == NULL) {
    mutex->blocked_tail = NULL;
    return 0;
  }

  tcb* blocked = mutex->blocked_head;

  do {
    blocked->status = READY;
    enqueue(blocked);
    blocked = blocked->queue_next;
  } while(blocked != NULL);

  mutex->locked = 0;
  mutex->blocked_head = NULL;
  mutex->blocked_tail = NULL;
  return 0;
}
  
//DO NOT MODIFY THIS FUNCTION
/* Function to print global statistics. Do not modify this function.*/
void print_app_stats(void) {
  fprintf(stderr, "Total context switches %ld \n", tot_cntx_switches);
  fprintf(stderr, "Average turnaround time %lf \n", avg_turn_time);
  fprintf(stderr, "Average response time  %lf \n", avg_resp_time);
}
