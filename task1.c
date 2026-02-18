#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 3

void* thread_print(void* arg) {
  printf("Thread [%d] is running\n", *(int*)arg);
  return NULL;
}

int main() {
  pthread_t threads[NUM_THREADS];
    
  for (int i = 0; i < NUM_THREADS; ++i) {
    if (pthread_create(&threads[i], NULL, thread_print, (void*)&i) != 0) {
      perror("Failed to create a thread\n");
      exit(EXIT_FAILURE);
    }
  }
    
  for (int i = 0; i < NUM_THREADS; ++i) {
    pthread_join(threads[i], NULL);
  }

  return 0;
}
