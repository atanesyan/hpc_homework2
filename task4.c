#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define SIZE 20 * 1000 * 1000
#define NUM_THREADS 4

bool isPrime(long long n) {
  if (n <= 1) {
    return false;
  }
  if (n <= 3) {
    return true;
  }
  if ((n % 2 == 0) || (n % 3 == 0)) {
    return false;
  }
    
  for (int i = 5; i * i <= n; i += 6) {
    if ((n % i) == 0 || (n % (i + 2)) == 0) {
      return false;
    } 
  }
    
  return true;
}

double get_time_diff(struct timespec start, struct timespec end) {
  return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}

typedef struct {
  long long start;
  long long end;
  int res;
} ThreadData;

void* count_primes(void* arg) {
  ThreadData* data = (ThreadData*)arg;
  int count = 0;
  for (long long i = data->start; i < data->end; ++i) {
    if (isPrime(i)) {
      ++count;
    }
  }
  data->res = count;

  return NULL;
}

int main() {
  struct timespec start, end;
  clock_gettime(CLOCK_MONOTONIC, &start);
  int count = 0;
  for (long long i = 0; i < SIZE; ++i) {
    if (isPrime(i)) {
      ++count;
    }
  }
  clock_gettime(CLOCK_MONOTONIC, &end);
  printf("Sequential count: %d | Time: %.6f seconds\n", count, get_time_diff(start, end));

  clock_gettime(CLOCK_MONOTONIC, &start);
  pthread_t threads[NUM_THREADS];
  ThreadData thread_args[NUM_THREADS];
  int chunk_size = SIZE / NUM_THREADS;

  for (long long i = 0; i < NUM_THREADS; ++i) {
    thread_args[i].start = i * chunk_size;
    thread_args[i].end = (i == (NUM_THREADS - 1))? SIZE : (i + 1) * chunk_size;
    thread_args[i].res = 0; // potentially useless
    pthread_create(&threads[i], NULL, count_primes, &thread_args[i]);
  }

  for (int i = 0; i < NUM_THREADS; ++i) {
    pthread_join(threads[i], NULL);
  }

  count = 0;
  for (int i = 0; i < NUM_THREADS; ++i) {
    count += thread_args[i].res;
  }
  clock_gettime(CLOCK_MONOTONIC, &end);
  printf("Parrallel count: %d | Time: %.6f seconds\n", count, get_time_diff(start, end));

  return 0;
}
