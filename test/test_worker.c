  #include "../include/thread-worker.h"
  #include "../include/queue.h"
  #include "../include/tracker.h"
  #include <assert.h>

  void* dummy_cb(void* arg) {
    printf("dummy running\n");
    return NULL;
  }

  void test_worker_create() { 
    printf("TEST_WORKER_CREATE_ENQUEUES_TCB_ON_QUEUE_AND_TRACKER...");

    queue_head = NULL;
    queue_tail = NULL;
    tracker_head = NULL;
    tracker_tail = NULL;
    worker_t thread_id;
    int ret_code;

    ret_code = worker_create(&thread_id, NULL, dummy_cb, NULL);
    assert(ret_code  == 0);
    assert(thread_id == 0);
    assert(queue_head->thread_id == 0);
    assert(tracker_head != NULL);
    assert(tracker_head->thread_id == 0);

    ret_code = worker_create(&thread_id, NULL, dummy_cb, NULL);
    assert(ret_code  == 0);
    assert(thread_id == 1);
    assert(queue_head->thread_id == 0);
    assert(queue_head->queue_next->thread_id == 1);
    assert(tracker_head->tracker_next->thread_id == 1);

    ret_code = worker_create(&thread_id, NULL, dummy_cb, NULL);
    assert(ret_code  == 0);
    assert(thread_id == 2);
    assert(queue_head->thread_id == 0);
    assert(queue_head->queue_next->thread_id == 1);
    assert(queue_head->queue_next->queue_next->thread_id == 2);
    assert(tracker_head->tracker_next->tracker_next->thread_id == 2);
    assert(queue_tail->thread_id == 2);
    assert(tracker_tail->thread_id == 2);

    printf("PASS\n");
  }

  int main() {
    test_worker_create();
    return 0;
  }
