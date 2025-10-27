#ifndef QUEUE_H
#define QUEUE_H

#include "../thread-worker.h"

extern tcb* queue_head;
extern tcb* queue_tail;

void enqueue(tcb* thread);
tcb* dequeue();
void remove_from_queue(tcb* thread);
tcb* find_min_elapsed();

#endif 
