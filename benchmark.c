#define _POSIX_C_SOURCE 200809L
#include "benchmark.h"
#include "memtrack.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/resource.h>

const Algo ALGOS[NB_ALGOS] = {
    { "Bubble Sort",    bubble_sort,    1 },
    { "Selection Sort", selection_sort, 1 },
    { "Insertion Sort", insertion_sort, 1 },
    { "Merge Sort",     merge_sort,     0 },
    { "Quick Sort",     quick_sort,     0 },
};

static double now_wall_ms(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1e6;
}

static double now_cpu_ms(void)
{
    return (double)clock() * 1000.0 / CLOCKS_PER_SEC;
}

/* Process peak resident set size (getrusage, ru_maxrss is in KB on Linux). */
long peak_rss_kb(void)
{
    struct rusage ru;
    getrusage(RUSAGE_SELF, &ru);
    return ru.ru_maxrss;
}

/* 5 runs normally; fewer for the very slow O(n^2) sorts on big inputs. */
int runs_for(const Algo *algo, size_t n)
{
    if (algo->quadratic && n >= 100000) return 1;
    if (algo->quadratic && n >= 50000)  return 3;
    return 5;
}

/* Sorts a fresh copy of src `runs` times; only the sort call is timed. */
Result benchmark_run(sort_fn fn, const int *src, size_t n, int runs)
{
    Result r = { 0, 0, 0, runs, 1 };
    int *work = malloc(n * sizeof(int));
    if (!work) { r.correct = 0; return r; }

    for (int k = 0; k < runs; k++) {
        char stack_marker;
        copy_array(work, src, n);            /* not measured */
        memtrack_reset(&stack_marker);

        double w0 = now_wall_ms(), c0 = now_cpu_ms();
        fn(work, n);
        double w1 = now_wall_ms(), c1 = now_cpu_ms();

        r.wall_ms += w1 - w0;
        r.cpu_ms  += c1 - c0;
        double kb = memtrack_peak_bytes() / 1024.0;
        if (kb > r.mem_kb) r.mem_kb = kb;
        if (!is_sorted(work, n)) r.correct = 0;
    }
    r.wall_ms /= runs;
    r.cpu_ms  /= runs;
    free(work);
    return r;
}

void csv_header(FILE *csv)
{
    fprintf(csv, "algorithm,input,size,runs,wall_ms,cpu_ms,memory_kb,rank,correct\n");
}

/* Runs the five algorithms on the SAME data and prints a ranked table. */
void compare_all(size_t n, InputType type, unsigned long long seed, FILE *csv)
{
    int *data = malloc(n * sizeof(int));
    if (!data) { puts("Out of memory."); return; }
    generate_array(data, n, type, seed);

    Result res[NB_ALGOS];
    int    done[NB_ALGOS];
    for (int i = 0; i < NB_ALGOS; i++) {
        done[i] = !(ALGOS[i].quadratic && n > QUADRATIC_LIMIT);
        if (done[i]) res[i] = benchmark_run(ALGOS[i].fn, data, n, runs_for(&ALGOS[i], n));
    }

    int rank[NB_ALGOS];
    for (int i = 0; i < NB_ALGOS; i++) {
        rank[i] = 0;
        if (!done[i]) continue;
        rank[i] = 1;
        for (int j = 0; j < NB_ALGOS; j++)
            if (done[j] && res[j].wall_ms < res[i].wall_ms) rank[i]++;
    }

    printf("\nInput: %s | size: %zu | seed: %llu\n", input_type_name(type), n, seed);
    printf("%-16s %10s %14s %12s %13s %5s\n",
           "Algorithm", "Array size", "Wall time (ms)", "CPU (ms)", "Memory (KB)", "Rank");
    for (int i = 0; i < NB_ALGOS; i++) {
        if (done[i]) {
            printf("%-16s %10zu %14.3f %12.3f %13.1f %5d%s\n", ALGOS[i].name, n,
                   res[i].wall_ms, res[i].cpu_ms, res[i].mem_kb, rank[i],
                   res[i].correct ? "" : "  !! NOT SORTED");
            if (csv)
                fprintf(csv, "%s,%s,%zu,%d,%.4f,%.4f,%.2f,%d,%d\n", ALGOS[i].name,
                        input_type_name(type), n, res[i].runs, res[i].wall_ms,
                        res[i].cpu_ms, res[i].mem_kb, rank[i], res[i].correct);
        } else {
            printf("%-16s %10zu %14s %12s %13s %5s\n", ALGOS[i].name, n,
                   "skipped", "-", "-", "-");
        }
    }
    free(data);
}

/* All input types x all sizes -> CSV file (used for the report/charts). */
void run_full_benchmark(const char *csv_path)
{
    static const size_t sizes[] = { 1000, 5000, 10000, 50000, 100000 };
    FILE *csv = fopen(csv_path, "w");
    if (!csv) { perror("Cannot open CSV file"); return; }
    csv_header(csv);
    for (int t = 0; t < INPUT_TYPES; t++)
        for (size_t s = 0; s < sizeof sizes / sizeof sizes[0]; s++)
            compare_all(sizes[s], (InputType)t, 42, csv);
    fclose(csv);
    printf("\nResults written to %s\n", csv_path);
}

/* Big array example: 100 000 random integers, fixed seed 42. */
void big_array_example(void)
{
    const size_t n = 100000;
    const unsigned long long seed = 42;
    int *data = malloc(n * sizeof(int));
    if (!data) { puts("Out of memory."); return; }
    generate_array(data, n, INPUT_RANDOM, seed);

    printf("\n=== Big array example: %zu random integers, seed %llu ===\n", n, seed);
    print_array("First elements:", data, n);
    printf("%-16s %5s %14s %12s %13s\n", "Algorithm", "Runs",
           "Wall time (ms)", "CPU (ms)", "Memory (KB)");
    for (int i = 0; i < NB_ALGOS; i++) {
        int runs = runs_for(&ALGOS[i], n);
        Result r = benchmark_run(ALGOS[i].fn, data, n, runs);
        printf("%-16s %5d %14.3f %12.3f %13.1f%s\n", ALGOS[i].name, runs,
               r.wall_ms, r.cpu_ms, r.mem_kb, r.correct ? "" : "  !! NOT SORTED");
    }
    printf("Process peak RSS (getrusage): %.1f MB\n", peak_rss_kb() / 1024.0);
    free(data);
}
