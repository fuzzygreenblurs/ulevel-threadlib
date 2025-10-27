#include "../include/timer.h"
#include "../include/thread-worker.h"
#include  <assert.h>

void test_isr_increments_quantum_counter() {
  printf("\nTIMER: TEST_ISR_INCREMENTS_ELAPSED_QUANTUMS_COUNTER...");
  elapsed_quantums = 0; 
  current = NULL;

  timer_isr();
  timer_isr();
  timer_isr();
  timer_isr();

  assert(elapsed_quantums == 4);

  printf("PASS\n");
}

int main() {
  test_isr_increments_quantum_counter();
  return 0; 
}
