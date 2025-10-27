#include <assert.h>
#include "../include/tracker.h"
#include "../include/thread-worker.h"
#include "../include/test_helpers.h"

void test_track() {
  printf("\nTRACKER: TEST_THREAD_LIFECYCLE_TRACKER...");
  tracker_head = NULL;
  tracker_tail = NULL;

  tcb* thread = (tcb*)calloc(1, sizeof(tcb));
  track(thread); 

  assert(tracker_head == thread);
  assert(tracker_tail == thread);
  assert(tracker_head->tracker_next == NULL); 

  tcb* thread_2 = (tcb*)calloc(1, sizeof(tcb));
  track(thread_2); 

  assert(tracker_head->tracker_next == thread_2);
  assert(tracker_tail == thread_2);
  assert(tracker_tail->tracker_next == NULL);

  free(thread);
  printf("PASS");
}

void test_track_multiple_threads() {
  printf("\nTRACKER: TEST_TRACK_MULTIPLE_THREADS...");

  tracker_head = NULL;
  tracker_tail = NULL;

  tcb* t0 = (tcb*)malloc(sizeof(tcb));
  track(t0);
  assert(tracker_head == t0);
  assert(tracker_tail == t0);
  assert(tracker_tail->tracker_next == NULL);

  tcb* t1 = (tcb*)malloc(sizeof(tcb));
  track(t1);
  assert(tracker_head == t0);
  assert(tracker_tail == t1);
  assert(tracker_tail->tracker_next == NULL);

  tcb* t2 = (tcb*)malloc(sizeof(tcb));
  track(t2);
  assert(tracker_head == t0);
  assert(tracker_tail == t2);
  assert(tracker_head->tracker_next == t1);
  assert(t1->tracker_next == t2);
  assert(tracker_tail->tracker_next == NULL);
  
  printf("PASS");
}

void test_find_thread_tracker() {
  printf("\nTRACKER: TEST_FIND_THREAD_TRACKER...");
  
  tracker_head = NULL;
  tracker_tail = NULL;

  tcb* threads[4];
  for(int i = 0; i < 4; i++) {
    tcb* t = (tcb*)malloc(sizeof(tcb));
    threads[i] = t;
    threads[i]->thread_id = i; 
  }

  tcb* target = find_thread_tracker(1, NULL);
  assert(target == NULL);

  track(threads[0]);
  track(threads[1]);
  track(threads[2]);

  target = find_thread_tracker(1, NULL);
  assert(target == threads[1]);

  target = find_thread_tracker(3, NULL);
  assert(target == NULL);
  
  printf("PASS");
}
int main() {
  test_track();
  test_track_multiple_threads();
  test_find_thread_tracker();
  
  return 0;
}
