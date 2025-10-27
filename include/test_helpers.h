#ifndef TEST_HELPERS
#define TEST_HELPERS

#include "thread-worker.h"

void reset_queue();
tcb** create_queue(int n);
void free_threads(tcb** threads, int n);
#endif 
