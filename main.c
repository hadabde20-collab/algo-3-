#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "benchmark.h"

#define SEED 42

/* ---------- robust input helpers (never crash on bad input) ---------- */
static int read_line(char *buf, size_t size)
{
    if (!fgets(buf, (int)size, stdin)) return 0;          /* EOF */
    if (!strchr(buf, '\n')) {                              /* line too long: flush */
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {}
    }
    return 1;
}

/* Returns 1 and stores the value if the user typed a valid integer. */
static int read_long(const char *prompt, long *out)
{
    char line[64], *end;
    printf("%s", prompt);
    fflush(stdout);
    if (!read_line(line, sizeof line)) { *out = 0; return -1; }   /* EOF */
    errno = 0;
    long v = strtol(line, &end, 10);
    if (end == line || errno == ERANGE) return 0;        /* no digits / overflow */
    while (*end == ' ' || *end == '\t' || *end == '\n') end++;
    if (*end != '\0') return 0;                          /* trailing garbage */
    *out = v;
    return 1;
}

static int ask_choice(const char *prompt, long lo, long hi, long *out)
{
    for (;;) {
        int st = read_long(prompt, out);
        if (st < 0) return 0;
        if (st == 1 && *out >= lo && *out <= hi) return 1;
        printf("  Invalid input: enter a number between %ld and %ld.\n", lo, hi);
    }
}

static int ask_size(size_t *n)
{
    long c;
    puts("\nArray size:\n  1. 1 000\n  2. 10 000\n  3. 100 000\n  4. Custom");
    if (!ask_choice("Your choice: ", 1, 4, &c)) return 0;
    if (c == 1) *n = 1000;
    else if (c == 2) *n = 10000;
    else if (c == 3) *n = 100000;
    else {
        long v;
        if (!ask_choice("Enter size (1 to 5 000 000): ", 1, 5000000, &v)) return 0;
        *n = (size_t)v;
    }
    return 1;
}

static int ask_type(InputType *t)
{
    long c;
    puts("\nInput type:\n  1. Random\n  2. Already sorted\n  3. Reverse-sorted\n  4. Many duplicates");
    if (!ask_choice("Your choice: ", 1, 4, &c)) return 0;
    *t = (InputType)(c - 1);
    return 1;
}

static void menu_single(int idx)
{
    size_t n; InputType t;
    if (!ask_size(&n) || !ask_type(&t)) return;
    const Algo *al = &ALGOS[idx];
    if (al->quadratic && n >= 100000)
        puts("  (warning: this O(n^2) algorithm may take several seconds on this size)");

    int *data = malloc(n * sizeof(int));
    if (!data) { puts("Out of memory."); return; }
    generate_array(data, n, t, SEED);

    int runs = runs_for(al, n);
    if (n <= 20) print_array("Before:", data, n);
    Result r = benchmark_run(al->fn, data, n, runs);
    if (n <= 20) {                       /* show the sorted result for small n */
        int *tmp = malloc(n * sizeof(int));
        if (tmp) { copy_array(tmp, data, n); al->fn(tmp, n); print_array("After: ", tmp, n); free(tmp); }
    }
    printf("\n%s | %s | n = %zu | %d run(s), average:\n", al->name, input_type_name(t), n, runs);
    printf("  Wall-clock time : %.3f ms\n", r.wall_ms);
    printf("  CPU time        : %.3f ms\n", r.cpu_ms);
    printf("  Peak memory     : %.1f KB (extra heap + stack used by the sort)\n", r.mem_kb);
    printf("  Result sorted   : %s\n", r.correct ? "yes" : "NO (bug!)");
    free(data);
}

static void menu_compare(void)
{
    InputType t; long c;
    if (!ask_type(&t)) return;
    puts("\nSizes:\n  1. Quick   (1 000, 5 000, 10 000)\n  2. Full    (1 000, 5 000, 10 000, 50 000, 100 000)");
    if (!ask_choice("Your choice: ", 1, 2, &c)) return;
    static const size_t sizes[] = { 1000, 5000, 10000, 50000, 100000 };
    size_t count = (c == 1) ? 3 : 5;

    FILE *csv = fopen("results/compare_menu.csv", "w");     /* NULL is tolerated */
    if (csv) csv_header(csv);
    for (size_t i = 0; i < count; i++) compare_all(sizes[i], t, SEED, csv);
    if (csv) { fclose(csv); puts("\nResults saved in results/compare_menu.csv"); }
    printf("Note: O(n^2) algorithms are skipped above %d elements.\n", QUADRATIC_LIMIT);
}

static void print_menu(void)
{
    puts("\n=========== SORTING ALGORITHMS ===========");
    puts("1. Bubble Sort");
    puts("2. Selection Sort");
    puts("3. Insertion Sort");
    puts("4. Merge Sort");
    puts("5. Quick Sort");
    puts("6. Compare all algorithms");
    puts("7. Big array example (100 000 integers)");
    puts("0. Exit");
}

int main(int argc, char **argv)
{
    if (argc >= 2 && strcmp(argv[1], "--bench") == 0) {      /* batch mode */
        run_full_benchmark(argc >= 3 ? argv[2] : "results/benchmark.csv");
        return 0;
    }
    if (argc >= 2 && strcmp(argv[1], "--big") == 0) { big_array_example(); return 0; }

    for (;;) {
        long c;
        print_menu();
        int st = read_long("Your choice: ", &c);
        if (st < 0) { puts("\nBye."); break; }                /* EOF */
        if (st == 0 || c < 0 || c > 7) { puts("Invalid choice, try again."); continue; }
        if (c == 0) { puts("Bye."); break; }
        if (c >= 1 && c <= 5) menu_single((int)c - 1);
        else if (c == 6)      menu_compare();
        else                  big_array_example();
    }
    return 0;
}
