#include "../include/thread-worker.h"
#include "../include/queue.h"
#include "../include/tracker.h"
#include "../include/timer.h"
#include <ucontext.h>

static void sched_psjf() {
  if(current && current->status != TERMINATED && 
    current->status != BLOCKED) {

    current->elapsed_time += 1;
    current->status = READY;
    enqueue(current);
  }

  tcb* min_elapsed = find_min_elapsed();
  if(min_elapsed == NULL) return;
  
  if(min_elapsed->first_run_time == -1) {
    min_elapsed->first_run_time = elapsed_quantums;
  }
  remove_from_queue(min_elapsed);
  
  current = min_elapsed;
  current->status = SCHEDULED;
swapcontext(&scheduler_context, &current->context);
}


/* Preemptive MLFQ scheduling algorithm */
static void sched_mlfq() {
	// - your own implementation of MLFQ
	// (feel free to modify arguments and return types)

	// YOUR CODE HERE

	/* Step-by-step guidances */
	// Step1: Calculate the time current thread actually ran
	// Step2.1: If current thread uses up its allotment, demote it to the low priority queue (Rule 4)
	// Step2.2: Otherwise, push the thread back to its origin queue
	// Step3: If time period S passes, promote all threads to the topmost queue (Rule 5)
	// Step4: Apply RR on the topmost queue with entries and run next thread
}

/* Completely fair scheduling algorithm */
static void sched_cfs(){
	// - your own implementation of CFS
	// (feel free to modify arguments and return types)

	// YOUR CODE HERE

	/* Step-by-step guidances */

	// Step1: Update current thread's vruntime by adding the time it actually ran
	// Step2: Insert current thread into the runqueue (min heap)
	// Step3: Pop the runqueue to get the thread with a minimum vruntime
	// Step4: Calculate time slice based on target_latency (TARGET_LATENCY), number of threads within the runqueue
	// Step5: If the ideal time slice is smaller than minimum_granularity (MIN_SCHED_GRN), use MIN_SCHED_GRN instead
	// Step5: Setup next time interrupt based on the time slice
	// Step6: Run the selected thread
}


/* scheduler */
// - invoke scheduling algorithms according to the policy (PSJF or MLFQ or CFS)
void schedule() {
	// every time a timer interrupt occurs, your worker thread library 
	// should be contexted switched from a thread context to this 
	// schedule() function
	
  while(1) {
    if(queue_head == NULL) {
      swapcontext(&scheduler_context, &main_context);
    }

    tot_cntx_switches++;
    #if defined(PSJF)
      sched_psjf();
    #elif defined(MLFQ)
      sched_mlfq();
    #elif defined(CFS)
      sched_cfs();  
    #else
      # error "Define one of PSJF, MLFQ, or CFS when compiling. e.g. make SCHED=MLFQ"
    #endif

    if(current != NULL && 
       current->status == TERMINATED && 
       current->joinable == 0) {
      
      tcb* prev;
      tcb* zombie = find_thread_tracker(current->thread_id, &prev);
      if(zombie) {
        untrack(zombie, prev);
        current = NULL;
      }
    }
  }
}


