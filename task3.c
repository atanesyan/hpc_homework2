#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 50 * 1000 * 1000
#define NUM_THREADS 4

double get_time_diff(struct timespec start, struct timespec end) {
  return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}

typedef struct {
  int* arr;
  int start;
  int end;
  int res;
} ThreadData;

void* count_max(void* arg) {
  ThreadData* data = (ThreadData*)arg;
  int max = data->arr[data->start];
  for (int i = data->start + 1; i < data->end; ++i) {
    if (max < data->arr[i]) {
      max = data->arr[i];
    }
  }
  data->res = max;

  return NULL;
}

int main() {
  int* arr = malloc(SIZE * sizeof(int));
  srand(time(NULL));
  for (int i = 0; i < SIZE; ++i) {
    arr[i] = rand() % 200 - 100;
  }

  struct timespec start, end;

  clock_gettime(CLOCK_MONOTONIC, &start);
  int max = arr[0];
  for (int i = 1; i < SIZE; ++i) {
    if (max < arr[i]) {
      max = arr[i];
    }
  }
  clock_gettime(CLOCK_MONOTONIC, &end);
  printf("Sequential max: %d | Time: %.6f seconds\n", max, get_time_diff(start, end));

  clock_gettime(CLOCK_MONOTONIC, &start);
  pthread_t threads[NUM_THREADS];
  ThreadData thread_args[NUM_THREADS];
  int chunk_size = SIZE / NUM_THREADS;

  for (int i = 0; i < NUM_THREADS; ++i) {
    thread_args[i].arr = arr;
    thread_args[i].start = i * chunk_size;
    thread_args[i].end = (i == (NUM_THREADS - 1))? SIZE : (i + 1) * chunk_size;
    thread_args[i].res = 0; // potentially useless
    pthread_create(&threads[i], NULL, count_max, &thread_args[i]);
  }

  for (int i = 0; i < NUM_THREADS; ++i) {
    pthread_join(threads[i], NULL);
  }
  free(arr);

  max = thread_args[0].res;
  for (int i = 1; i < NUM_THREADS; ++i) {
    if (max < thread_args[i].res) {
      max = thread_args[i].res;
    }
  }
  clock_gettime(CLOCK_MONOTONIC, &end);
  printf("Parrallel max: %d | Time: %.6f seconds\n", max, get_time_diff(start, end));

  return 0;
}
