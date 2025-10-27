#include <limits.h>
#include "queue.h"
#include "thread-worker.h"

tcb* queue_head = NULL;
tcb* queue_tail = NULL;

void enqueue(tcb* thread) {
  thread->queue_next = NULL;

  if(queue_head == NULL) {
    queue_head = thread;
    queue_tail = thread;
  } else {
    queue_tail->queue_next = thread;
    queue_tail = thread;
  }

  //printf("\nafter: head=%p tail=%p\n", queue_head, queue_tail);
}

tcb* dequeue() {
  if(queue_head == NULL) {
    return NULL;
  } 

  tcb* thread = queue_head;
  queue_head = thread->queue_next;
 
  if(queue_head == NULL) {
    queue_tail = NULL;
  }
  
  thread->queue_next = NULL;
  return thread;
}

void remove_from_queue(tcb* thread) {
  if(queue_head == NULL) return;
  
  if(thread == queue_head) {
    queue_head = queue_head->queue_next;
    if(queue_head == NULL) { 
      queue_tail = NULL;
    }
    thread->queue_next = NULL;
    return;
  }

  tcb* candidate = queue_head->queue_next;
  tcb* prev = queue_head;

  do {
    if(candidate == thread) {
      if(candidate == queue_tail) {
        prev->queue_next = NULL;
        queue_tail = prev;
      } else {
        prev->queue_next = thread->queue_next;
      } 

      thread->queue_next = NULL;
      return;
    } 

    prev = candidate;
    candidate = candidate->queue_next;
  } while(candidate != NULL);
}

tcb* find_min_elapsed() {
  if (queue_head == NULL) return NULL; 

  int min_elapsed = INT_MAX;
  tcb* min_thread = NULL;

  tcb* candidate = queue_head;
  while(candidate != NULL) {
    if(candidate->elapsed_time < min_elapsed) {
      min_elapsed = candidate->elapsed_time;
      min_thread = candidate;
    }

    candidate = candidate->queue_next;
  }

  return min_thread;
}
