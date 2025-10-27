#include "../include/thread-worker.h"
#include "../include/queue.h"
#include <assert.h>

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

void test_enqueue_single() {
  printf("\nQUEUE: TEST_ENQUEUE_SINGLE...");
  queue_head = NULL;
  queue_tail = NULL;

  tcb* t0 = (tcb*)calloc(1, sizeof(tcb)); 
  enqueue(t0);

  assert(queue_head == t0);
  assert(queue_tail == t0);
  assert(queue_tail->queue_next == NULL);

  printf("PASS");

  free(t0);
}

void test_enqueue_multiple() {
  printf("\nQUEUE: TEST_ENQUEUE_MULTIPLE...");
 
  int num_threads = 3;
  tcb** threads = create_queue(num_threads);

  assert(queue_head == threads[0]);
  assert(queue_head->queue_next == threads[1]);
  assert(queue_head->queue_next->queue_next == threads[2]);
  assert(queue_tail == threads[2]);
  assert(queue_tail->queue_next == NULL);

  free_threads(threads, num_threads);
  printf("PASS");
}

void test_dequeue_single() {
  printf("\nQUEUE: TEST_DEQUEUE_SINGLE...");
 
  int num_threads = 3;
  tcb** threads = create_queue(num_threads);

  assert(queue_head == threads[0]);
  assert(queue_head->queue_next == threads[1]);
  
  tcb* dequeued = dequeue();

  assert(dequeued == threads[0]);
  assert(queue_head == threads[1]);
  assert(queue_head->queue_next == threads[2]);
  
  free_threads(threads, num_threads);
  printf("PASS");
}

void test_dequeue_past_empty() {
  printf("\nQUEUE: TEST_DEQUEUE_PAST_EMPTY...");
 
  int num_threads = 3;
  tcb** threads = create_queue(num_threads);

  assert(threads[0] == queue_head);
  tcb* dequeued;
  
  dequeued = dequeue();
  dequeued = dequeue();
  dequeued = dequeue();
  assert(dequeued == threads[2]);
  assert(queue_head == NULL);
  assert(queue_tail == NULL); 
  
  dequeued = dequeue();
  assert(dequeued == NULL);

  free_threads(threads, num_threads);
  printf("PASS");
}

void test_remove_middle() {
  printf("\nQUEUE: TEST_REMOVE_FROM_ARBITRARY_POSITION...");
  int num_threads = 4;
  tcb** threads = create_queue(num_threads);

  tcb* target = threads[2];
  assert(queue_head->queue_next->queue_next == threads[2]);
  remove_from_queue(target);
  assert(queue_head->queue_next->queue_next == threads[3]);

  free_threads(threads, num_threads);
  printf("PASS");
}

void test_remove_head() {
  printf("\nQUEUE: TEST_REMOVE_HEAD...");
  int num_threads = 3;
  tcb** threads = create_queue(num_threads);

  assert(queue_head == threads[0]);
  remove_from_queue(queue_head);
  assert(queue_head == threads[1]);

  free_threads(threads, num_threads);
  printf("PASS");
}

void test_remove_tail() {
  printf("\nQUEUE: TEST_REMOVE_TAIL...");
  int num_threads = 3;
  tcb** threads = create_queue(num_threads);

  assert(queue_tail == threads[2]);
  remove_from_queue(queue_tail);
  assert(queue_tail == threads[1]);
  assert(queue_tail->queue_next == NULL);

  free_threads(threads, num_threads);
  printf("PASS");
}

void test_find_min_elapsed() {
  printf("\nQUEUE: TEST_FIND_MIN_ELAPSED...");
  int num_threads = 5;
  tcb** threads = create_queue(num_threads);
  
  threads[0]->elapsed_time = 15; 
  threads[1]->elapsed_time = 4; 
  threads[2]->elapsed_time = 1; 
  threads[3]->elapsed_time = 0; 
  threads[4]->elapsed_time = 2; 

  tcb* min_elapsed = find_min_elapsed();
  assert(min_elapsed == threads[3]);

  free_threads(threads, num_threads);
  printf("PASS");
} 

void test_return_first_thread_if_tied() {
  printf("\nQUEUE: TEST_RETURN_FIRST_THREAD_IF_TIED...");
  int num_threads = 5;
  tcb** threads = create_queue(num_threads);
  
  for(int i = 0; i < num_threads; i++) {
    threads[i]->elapsed_time = 0;
  }

  tcb* min_elapsed = find_min_elapsed();
  assert(min_elapsed == threads[0]);

  free_threads(threads, num_threads);
  printf("PASS");
} 

int main() {
  test_enqueue_single();
  test_enqueue_multiple();
  test_dequeue_single();
  test_dequeue_past_empty();
  test_remove_middle();
  test_remove_head();
  test_remove_tail();
  test_find_min_elapsed();
  test_return_first_thread_if_tied();
  return 0; 
}
