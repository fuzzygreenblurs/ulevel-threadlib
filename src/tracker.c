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
