  #include "../include/thread-worker.h"
  #include "../include/queue.h"
  #include "../include/tracker.h"
  #include <assert.h>

  void* dummy_cb(void* arg) {
    printf("dummy running\n");
    return NULL;
  }

  void test_worker_create() { 
    printf("TEST_WORKER_CREATE_ENQUEUES_TCB_ON_QUEUE_AND_TRACKER");
    queue_head = NULL;
    queue_tail = NULL;
    tracker_head = NULL;
    tracker_tail = NULL;
 
    worker_t thread_id;
    int res = worker_create(&thread_id, NULL, dummy_cb, NULL);
    assert(res == 0);
    assert(thread_id == 0);

    assert(queue_head->thread_id == 0);
    assert(tracker_head != NULL);
    assert(tracker_head->thread_id == 0);
    printf("PASS\n");
  }

  int main() {
    return 0;
  }
