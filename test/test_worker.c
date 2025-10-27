  #include "../thread-worker.h"
  #include "../include/queue.h"
  #include "../include/tracker.h"
  #include <assert.h>

  void* dummy_cb(void* arg) {
    return NULL;
  }

  void* dummy_cb_increment(void* arg) {
    //this is a purposely convoluted callback to show 
    //that type casting must match in size to avoid UB
    int* val = (int*)arg;
    return (void*)(long)(*val + 1);
  }

  void test_worker_create() { 
    printf("TRACKER: TEST_WORKER_CREATE_ENQUEUES_TCB_ON_QUEUE_AND_TRACKER...");

    queue_head = NULL;
    queue_tail = NULL;
    tracker_head = NULL;
    tracker_tail = NULL;
    worker_t thread_id;
    int retcode;

    retcode = worker_create(&thread_id, NULL, dummy_cb, NULL);
    assert(retcode  == 0);
    assert(thread_id == 0);
    assert(queue_head->thread_id == 0);
    assert(tracker_head != NULL);
    assert(tracker_head->thread_id == 0);

    retcode = worker_create(&thread_id, NULL, dummy_cb, NULL);
    assert(retcode  == 0);
    assert(thread_id == 1);
    assert(queue_head->thread_id == 0);
    assert(queue_head->queue_next->thread_id == 1);
    assert(tracker_head->tracker_next->thread_id == 1);

    retcode = worker_create(&thread_id, NULL, dummy_cb, NULL);
    assert(retcode  == 0);
    assert(thread_id == 2);
    assert(queue_head->thread_id == 0);
    assert(queue_head->queue_next->thread_id == 1);
    assert(queue_head->queue_next->queue_next->thread_id == 2);
    assert(tracker_head->tracker_next->tracker_next->thread_id == 2);
    assert(queue_tail->thread_id == 2);
    assert(tracker_tail->thread_id == 2);

    printf("PASS\n");
  }

//  void test_worker_yield() {
//    printf("TEST_WORKER_VOLUNTARILY_YIELDS_SUCCESSFULLY...");
//
//    print("PASS\n")
//  }

void test_worker_join() {
  printf("TRACKER; TEST_JOIN()_FORCES_CALLER_TO_WAIT_FOR_RETURN...");
  worker_t thread_id;
  int arg = 99;
  int retcode;
  void* retval = NULL;

  retcode = worker_create(&thread_id, NULL, dummy_cb_increment, &arg);
  assert(retcode == 0);

  assert(retval == NULL);
  retcode = worker_join(thread_id, &retval);
  assert(retcode == 0);
  assert((long)retval == 100);

  printf("PASS\n");
}
  
  
int main() {
  test_worker_create();
//    test_worker_yield();
  test_worker_join();
  return 0;
}
