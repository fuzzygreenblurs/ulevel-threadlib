#ifndef TRACKING_H
#define TRACKING_H

#include "thread-worker.h"

extern tcb* tracker_head;
extern tcb* tracker_tail;

void track(tcb* thread);
//void untrack(tcb* thread, tcb* prev);
//tcb* find_tracked_thread(worker_t thread, tcb* *prev_tracker);

#endif
