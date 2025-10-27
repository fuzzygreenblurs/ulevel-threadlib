#include "../thread-worker.h"
#include "../include/queue.h"

void reset_queue() {
  queue_head = NULL;
  queue_tail = NULL;
}

tcb** create_queue(int n) {
  reset_queue();

  tcb** threads = (tcb**)malloc(n * sizeof(tcb*));
  for(int i = 0; i < n; i++) {
    threads[i] = (tcb*)calloc(1, sizeof(tcb));
    threads[i]->thread_id = i;
    enqueue(threads[i]);
  }

  return threads;
}

void free_threads(tcb** threads, int n) {
  for(int i = 0; i < n; i++) {
    free(threads[i]);
  }

  free(threads);
}
