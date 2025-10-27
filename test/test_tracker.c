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

  printf("PASS");
}

int main() {
  test_track();
  return 0;
}
