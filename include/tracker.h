#ifndef TRACKER_H
#define TRACKER_H

#include "../thread-worker.h"

extern tcb* tracker_head;
extern tcb* tracker_tail;

void track(tcb* thread);
void untrack(tcb* thread, tcb* prev);
tcb* find_thread_tracker(worker_t thread, tcb* *prev_tracker);

#endif
