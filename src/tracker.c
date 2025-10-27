#include <stdlib.h>
#include "tracker.h"
#include "thread-worker.h"

tcb* tracked_head = NULL; 
tcb* tracked_tail = NULL;

void track(tcb* thread) {
  if(thread == NULL) return;

  thread->joinable = 1;
  if(tracked_head == NULL) {
    tracked_head = thread;
    tracked_tail = thread;
  } else {
    tracked_tail->tracked_next = thread;
    tracked_tail = thread;
  }

  thread->tracked_next = NULL;  
}
