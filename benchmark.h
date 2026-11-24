#ifndef BENCHMARK_H
#define BENCHMARK_H
#include <stdio.h>
#include "sorting.h"
#include "utils.h"

#define NB_ALGOS        5
#define QUADRATIC_LIMIT 50000      /* O(n^2) sorts are skipped above this size
                                      in the automatic comparison            */

typedef struct { const char *name; sort_fn fn; int quadratic; } Algo;
extern const Algo ALGOS[NB_ALGOS];

typedef struct {
    double wall_ms;   /* average wall-clock time  (clock_gettime)       */
    double cpu_ms;    /* average processor time   (clock)               */
    double mem_kb;    /* peak extra memory during the sort (heap+stack) */
    int    runs;
    int    correct;   /* 1 if the output was sorted on every run        */
} Result;

int    runs_for(const Algo *algo, size_t n);
Result benchmark_run(sort_fn fn, const int *src, size_t n, int runs);
void   compare_all(size_t n, InputType type, unsigned long long seed, FILE *csv);
void   run_full_benchmark(const char *csv_path);
void   big_array_example(void);
long   peak_rss_kb(void);
void   csv_header(FILE *csv);
#endif
