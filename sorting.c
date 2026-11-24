#include "sorting.h"
#include "memtrack.h"
#include <string.h>

static void swap_int(int *x, int *y) { int t = *x; *x = *y; *y = t; }

/* ---------------- 1. Bubble Sort (with early-exit flag) ---------------- */
void bubble_sort(int *a, size_t n)
{
    if (n < 2) return;
    for (size_t pass = 0; pass < n - 1; pass++) {
        int swapped = 0;
        for (size_t j = 0; j < n - 1 - pass; j++) {
            if (a[j] > a[j + 1]) {
                swap_int(&a[j], &a[j + 1]);
                swapped = 1;
            }
        }
        if (!swapped) break;           /* already sorted: stop */
    }
}

/* ---------------- 2. Selection Sort ---------------- */
void selection_sort(int *a, size_t n)
{
    if (n < 2) return;
    for (size_t i = 0; i < n - 1; i++) {
        size_t min = i;
        for (size_t j = i + 1; j < n; j++)
            if (a[j] < a[min]) min = j;
        if (min != i) swap_int(&a[i], &a[min]);
    }
}

/* ---------------- 3. Insertion Sort ---------------- */
void insertion_sort(int *a, size_t n)
{
    for (size_t i = 1; i < n; i++) {
        int key = a[i];
        size_t j = i;
        while (j > 0 && a[j - 1] > key) {   /* shift larger elements right */
            a[j] = a[j - 1];
            j--;
        }
        a[j] = key;
    }
}

/* ---------------- 4. Merge Sort (top-down, one auxiliary buffer) ------- */
static void merge_rec(int *a, int *tmp, size_t lo, size_t hi)   /* [lo,hi) */
{
    STACK_PROBE();
    if (hi - lo < 2) return;
    size_t mid = lo + (hi - lo) / 2;
    merge_rec(a, tmp, lo, mid);
    merge_rec(a, tmp, mid, hi);

    size_t i = lo, j = mid, k = lo;
    while (i < mid && j < hi)
        tmp[k++] = (a[i] <= a[j]) ? a[i++] : a[j++];   /* <= : stable */
    while (i < mid) tmp[k++] = a[i++];
    while (j < hi)  tmp[k++] = a[j++];
    memcpy(a + lo, tmp + lo, (hi - lo) * sizeof(int));
}

void merge_sort(int *a, size_t n)
{
    if (n < 2) return;
    int *tmp = tracked_malloc(n * sizeof(int));
    if (!tmp) return;
    merge_rec(a, tmp, 0, n);
    tracked_free(tmp);
}

/* ---------------- 5. Quick Sort ----------------
 * Pivot strategy : MEDIAN-OF-THREE (first, middle, last) -> avoids the
 *                  O(n^2) behaviour on sorted / reverse-sorted input.
 * Partition      : 3-way (< pivot | == pivot | > pivot) -> linear time on
 *                  arrays with many duplicates.
 * Recursion      : recurse on the SMALLER part, loop on the larger one ->
 *                  recursion depth <= log2(n), no stack overflow.        */
static int median3(int x, int y, int z)
{
    if ((x <= y && y <= z) || (z <= y && y <= x)) return y;
    if ((y <= x && x <= z) || (z <= x && x <= y)) return x;
    return z;
}

static void quick_rec(int *a, long lo, long hi)                 /* [lo,hi] */
{
    while (lo < hi) {
        STACK_PROBE();
        int pivot = median3(a[lo], a[lo + (hi - lo) / 2], a[hi]);
        long lt = lo, i = lo, gt = hi;
        while (i <= gt) {
            if (a[i] < pivot)      swap_int(&a[lt++], &a[i++]);
            else if (a[i] > pivot) swap_int(&a[i], &a[gt--]);
            else                   i++;
        }
        /* a[lo..lt-1] < pivot, a[lt..gt] == pivot, a[gt+1..hi] > pivot */
        if (lt - lo < hi - gt) { quick_rec(a, lo, lt - 1); lo = gt + 1; }
        else                   { quick_rec(a, gt + 1, hi); hi = lt - 1; }
    }
}

void quick_sort(int *a, size_t n)
{
    if (n < 2) return;
    quick_rec(a, 0, (long)n - 1);
}
