#ifndef #TEST_HELPERS
#define #TEST_HELPERS

#include "thread-worker.h"

void reset_queue();
tcb** create_queue(int n);
void free_threads(tch** threads, int n);
#endif 
