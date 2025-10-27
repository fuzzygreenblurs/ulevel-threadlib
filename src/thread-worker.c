  #include "../include/thread-worker.h"

  // Global variables
  tcb* current = NULL;
  ucontext_t scheduler_context;
  ucontext_t main_context;
  long tot_cntx_switches = 0;

  static int scheduler_initialized = 0;
