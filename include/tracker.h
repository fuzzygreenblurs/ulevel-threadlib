#ifndef TRACKING_H
#define TRACKING_H

#include "thread-worker.h"

extern tcb* tracked_head;
extern tcb* tracked_tail;

void track(tcb* thread);
//void untrack(tcb* thread, tcb* prev);
//tcb* find_tracked_thread(worker_t thread, tcb* *prev_tracker);

#endif
