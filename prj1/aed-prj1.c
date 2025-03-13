#include <stdint.h>

#include <stdio.h>
#include <stdlib.h>

#include <time.h>
#include <limits.h>

#include <unistd.h>
#include <sys/types.h>

#define TRUE 1
#define FALSE 0

#define RAND(a, b) ((a) + rand() % ((b) - (a) + 1))

void print_arr(int* arr, int len);
int comp(const void *a, const void *b);
void range_missing_fill(int* seq, size_t len, int beg);
void swap(int* a, int* b);
int partition(int arr[], int low, int high);

void quick_sort(int arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quick_sort(arr, low, pi - 1);
        quick_sort(arr, pi + 1, high);
    }
}

int range_check_missing_naive(int* seq, size_t len);
int range_check_missing_sort(int* seq, size_t len);
int range_check_missing_checksum(int* seq, size_t len);

int
range_check_missing_naive(int* seq, size_t len) {
  int min = *seq, max = *seq;
  for (int *ptr = seq, *end_ptr = seq + len; ptr != end_ptr; ptr++) {
    if (*ptr < min) min = *ptr; 
    if (*ptr > max) max = *ptr; 
  }

  int* ptr;
  int* end_ptr = seq + len;
  int found;

  int i;

  for (i = min; i <= max; i++) {

    /* Check if is in sequence */
    found = FALSE;
    for (ptr = seq; ptr != end_ptr; ptr++) {
      if (*ptr == i) {
        found = TRUE;
        break;
      }
    }

    if (found == FALSE) return i;
  }


  return TRUE;
}

int
range_check_missing_sort(int* seq, size_t len) {

  quick_sort(seq, 0, len);

  for (int* ptr_end = seq + len - 1; seq != ptr_end; seq++) {
    if (*(seq+1) != *seq + 1) return *seq + 1;
  }
  return TRUE;
}

int
range_check_missing_checksum(int* seq, size_t len) {

  int64_t min = seq[0];
  int64_t max = seq[0];

  int64_t arr_sum = 0;

  for (int *ptr = seq, *end_ptr = seq + len; ptr != end_ptr; ptr++) {

    if (*ptr < min) min = *ptr; 
    if (*ptr > max) max = *ptr; 
    
    arr_sum += *ptr;
  }

  /* The Gauss Sum Can Get Really Big */
  int64_t range_sum = (max*(max+1))/2  - min*(min-1)/2;

  return range_sum-arr_sum;
}

#define NMAX    1000000
#define NSTEP   50000
#define NSTART  50000

#define AVERAGE 10

clock_t start, end;
int missing_sorted = 0;
double time_sorted = 0;

int
main() {

  /* Random seed */
  srand(time(NULL));

  (void) printf("Array Size: %d\nIterations per Algorithm: %d\n", NMAX, AVERAGE);

  for (int n = NSTART; n <= NMAX; n+= NSTEP) {

    missing_sorted = 0;
    time_sorted = 0;

    for (int i = 0; i < AVERAGE; i++) {

      int random_seq[n];
      range_missing_fill(random_seq, n, RAND(-n, n) );

      start = clock();
      missing_naive = range_check_missing_naive(random_seq, NMAX);
      end = clock();
      time_naive += ((double) (end - start)*1000) / CLOCKS_PER_SEC;

      start = clock();
      missing_sum = range_check_missing_checksum(random_seq, n);
      end = clock();

      time_sorted += ((double) (end - start)*1000) / CLOCKS_PER_SEC;

      start = clock();
      missing_sorted = range_check_missing_sort(random_seq, n);
      end = clock();
      time_sorted += ((double) (end - start)*1000) / CLOCKS_PER_SEC;

      /*printf("| Algorithm | Missing | Avg. Time (ms) |\n| Naive     | %7d | %14.3lf |\n| Sorted    | %7d | %14.3lf |\n| Linear    | %7d | %14.3lf |\n", missing_naive, time_naive/i, missing_sorted, time_sorted/i, missing_sum, time_sum/i) ;*/
    }
    printf("%d\t%.4lf\n", n, time_sorted / AVERAGE);

  }

}

int
comp(const void *a, const void *b) {
  return (*(int *)a - *(int *)b);
}

void
print_arr(int* arr, int len) {
  for (int* ptr = arr; ptr != arr+len; ptr++) {
    printf("%d ", *ptr);
  }
  putc('\n', stdout);
}

void
range_missing_fill(int* seq, size_t len, int beg) {

  int *ptr; 
  int *end = seq + len;
  size_t i = beg;

  /* Increment twice at this address */
  int* skip_ptr = seq + RAND(1, len-2);

  /* O(n) fill the array*/
  for (ptr = seq; ptr != end; ptr++) {
    *ptr = i;
    i++;
    if (ptr == skip_ptr) i++;
  }

  /* Shuffle it arround in O(n) */
  for (size_t i = len - 1; i > 0; i--) {
    size_t j = RAND(0, i);

    /* Swap Elements */
    int temp = seq[i];
    seq[i] = seq[j];
    seq[j] = temp;
  }

}

int
partition(int arr[], int low, int high) {
    int pivot = arr[high];
    int i = low - 1;
    for (int j = low; j <= high - 1; j++) {
        if (arr[j] < pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);  
    return i + 1;
}

void
swap(int* a, int* b) {
    int t = *a;
    *a = *b;
    *b = t;
}
