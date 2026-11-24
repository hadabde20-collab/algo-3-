#include "utils.h"
#include <stdio.h>
#include <string.h>

static const char *NAMES[INPUT_TYPES] = { "random", "sorted", "reverse", "duplicates" };
const char *input_type_name(InputType t) { return NAMES[t]; }

/* Own xorshift64 generator: same sequence on every OS/compiler for a given
 * seed (rand() is implementation-defined, so it would not be reproducible). */
static unsigned long long next_rand(unsigned long long *s)
{
    unsigned long long x = *s;
    x ^= x << 13; x ^= x >> 7; x ^= x << 17;
    return *s = x;
}

void generate_array(int *a, size_t n, InputType t, unsigned long long seed)
{
    unsigned long long s = seed ? seed : 88172645463325252ULL;
    for (size_t i = 0; i < n; i++) {
        switch (t) {
        case INPUT_SORTED:     a[i] = (int)i;                         break;
        case INPUT_REVERSE:    a[i] = (int)(n - i);                   break;
        case INPUT_DUPLICATES: a[i] = (int)(next_rand(&s) % 100);     break;
        default:               a[i] = (int)(next_rand(&s) % 1000000); break;
        }
    }
}

void copy_array(int *dst, const int *src, size_t n)
{
    memcpy(dst, src, n * sizeof(int));
}

int is_sorted(const int *a, size_t n)
{
    for (size_t i = 1; i < n; i++)
        if (a[i - 1] > a[i]) return 0;
    return 1;
}

void print_array(const char *label, const int *a, size_t n)
{
    size_t show = n < 15 ? n : 15;
    printf("%s [", label);
    for (size_t i = 0; i < show; i++) printf("%s%d", i ? ", " : "", a[i]);
    if (show < n) printf(", ... (%zu elements)", n);
    printf("]\n");
}
