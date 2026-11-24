#ifndef SORTING_H
#define SORTING_H
#include <stddef.h>

/* The five algorithms: each sorts a[0..n-1] in ascending order, in place.
 * No built-in sort function is used here. */
void bubble_sort(int *a, size_t n);
void selection_sort(int *a, size_t n);
void insertion_sort(int *a, size_t n);
void merge_sort(int *a, size_t n);
void quick_sort(int *a, size_t n);

typedef void (*sort_fn)(int *, size_t);
#endif
