  #include "../include/thread-worker.h"
  #include "../include/timer.h"
  #include "../include/queue.h"
  #include "../include/tracker.h"
  #include "../include/scheduler.h"


  // Global variables
  tcb* current = NULL;
  ucontext_t scheduler_context;
  ucontext_t main_context;
  long tot_cntx_switches = 0;

  static int scheduler_initialized = 0;
  static worker_t latest_assigned_id = 0;

  void worker_exit(void* value_ptr);

  static void store_cb_retval() {
    void* ret = current->cb(current->cb_arg);
    worker_exit(ret);
  }

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

