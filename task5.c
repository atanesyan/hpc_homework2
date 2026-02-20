#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>

#define NUM_THREADS 4
#define ITERATIONS 5

void* heavy_work(void* arg) {
  long id = (long)arg;
  for (int i = 0; i < ITERATIONS; i++) {
    for (volatile long j = 0; j < 500000000; j++) {}
    int cpu = sched_getcpu();
    printf("Thread ID: %ld | Running on CPU Core: %d\n", id, cpu);
  }
    
  return NULL;
}

int main() {
  pthread_t threads[NUM_THREADS];

  printf("Starting %d threads. Watch for migrations...\n", NUM_THREADS);

  for (long i = 0; i < NUM_THREADS; i++) {
    pthread_create(&threads[i], NULL, heavy_work, (void*)i);
  }

  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_join(threads[i], NULL);
  }

  return 0;
}
