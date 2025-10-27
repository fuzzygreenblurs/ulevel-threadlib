#include <stdlib.h>
#include "../include/tracker.h"
#include "../include/thread-worker.h"

tcb* tracker_head = NULL; 
tcb* tracker_tail = NULL;

void track(tcb* thread) {
  if(thread == NULL) return;

  thread->joinable = 1;
  if(tracker_head == NULL) {
    tracker_head = thread;
    tracker_tail = thread;
  } else {
    tracker_tail->tracker_next = thread;
    tracker_tail = thread;
  }

  thread->tracker_next = NULL;  
}

tcb* find_thread_tracker(worker_t thread, tcb** prev_tracker) { 
  if(tracker_head == NULL) return NULL;

  tcb* candidate = tracker_head;
  tcb* prev = NULL;

  do {
    if(candidate->thread_id == thread) {
      // optional param prev_tracker can be provided
      if(prev_tracker != NULL) {
        *prev_tracker = prev;
      }

      return candidate;
    } 

    prev = candidate;
    candidate = candidate->tracker_next;
  } while(candidate != NULL);

  return NULL;
}

