/* Correctness tests: every algorithm is checked against the C library qsort
 * (qsort is used ONLY here, as a reference oracle). */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/sorting.h"
#include "../src/utils.h"

static int cmp(const void *a, const void *b)
{
    int x = *(const int *)a, y = *(const int *)b;
    return (x > y) - (x < y);
}

static int check(const char *name, sort_fn f, const int *src, size_t n, const char *what)
{
    int *a = malloc((n ? n : 1) * sizeof(int)), *ref = malloc((n ? n : 1) * sizeof(int));
    memcpy(a, src, n * sizeof(int));
    memcpy(ref, src, n * sizeof(int));
    f(a, n);
    qsort(ref, n, sizeof(int), cmp);
    int ok = (n == 0) || memcmp(a, ref, n * sizeof(int)) == 0;
    if (!ok) printf("FAIL  %-15s %s (n=%zu)\n", name, what, n);
    free(a); free(ref);
    return ok;
}

int main(void)
{
    const char *names[] = { "Bubble", "Selection", "Insertion", "Merge", "Quick" };
    sort_fn fns[] = { bubble_sort, selection_sort, insertion_sort, merge_sort, quick_sort };
    int failures = 0, total = 0;

    for (int k = 0; k < 5; k++) {
        /* edge cases */
        int one[] = { 7 }, two[] = { 2, 1 }, neg[] = { 3, -1, -5, 0, 3, -1, 9, -9 };
        int same[] = { 4, 4, 4, 4, 4 };
        total += 5;
        failures += !check(names[k], fns[k], one, 1, "single element");
        failures += !check(names[k], fns[k], NULL, 0, "empty array");
        failures += !check(names[k], fns[k], two, 2, "two elements");
        failures += !check(names[k], fns[k], neg, 8, "negative numbers");
        failures += !check(names[k], fns[k], same, 5, "all equal");

        /* generated arrays of many sizes and kinds */
        for (size_t n = 1; n <= 2000; n = n * 3 + 1)
            for (int t = 0; t < INPUT_TYPES; t++) {
                int *a = malloc(n * sizeof(int));
                generate_array(a, n, (InputType)t, 1234 + n);
                total++;
                failures += !check(names[k], fns[k], a, n, input_type_name((InputType)t));
                free(a);
            }
    }
    printf("%d tests, %d failure(s)\n", total, failures);
    return failures ? 1 : 0;
}
