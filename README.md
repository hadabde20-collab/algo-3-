# Sorting Algorithms in C

Assignment: *Sorting Algorithms - Implementation, Performance Measurement and Comparison*
Module: Algorithms and Data Structures - Dr. Sabri Ghazi - Universite Badji Mokhtar, Annaba

student : Abderrahmene Hadiby **Academic year:*2* 2026/2027

## 1. Language and measurement tools

- The project is written in C11 and compiled with GCC 13.3 (`-O2 -Wall -Wextra -pedantic`). It runs on Linux, macOS and WSL (POSIX).
- Wall-clock time is measured with `clock_gettime(CLOCK_MONOTONIC)`, and processor time with `clock()`.
- To measure memory, I wrote my own allocation wrapper (`tracked_malloc` / `tracked_free`) and added a stack probe (`__builtin_frame_address`). Together they give the exact peak of *extra* memory used during a sort.
  `getrusage()` only gives a peak for the whole process that never goes down, so I use it just once as a global check (`ru_maxrss`).
- Each algorithm sorts its own copy of the same data. Only the sort call is timed. The results are averaged over several runs, and every result is checked with `is_sorted()`.
- For the test data I wrote a xorshift64 generator with a fixed seed of 42, so the data is the same on any system.

## 2. Build and run

```bash
make                 # build ./sorting
./sorting            # interactive menu (1-5 single algorithm, 6 compare all, 7 big array, 0 exit)
./sorting --big      # big array example: 100 000 integers, seed 42
make bench           # all sizes x all input types -> results/benchmark.csv (about 4 min)
make test            # 165 correctness tests
python3 results/make_charts.py   # charts (needs matplotlib)
```

## 3. Algorithms and complexity

| Algorithm | Best | Average | Worst | Extra space | Stable |
|---|---|---|---|---|---|
| Bubble Sort (early exit) | O(n) | O(n^2) | O(n^2) | O(1) | yes |
| Selection Sort | O(n^2) | O(n^2) | O(n^2) | O(1) | no |
| Insertion Sort | O(n) | O(n^2) | O(n^2) | O(1) | yes |
| Merge Sort | O(n log n) | O(n log n) | O(n log n) | O(n) | yes |
| Quick Sort (median-of-3, 3-way) | O(n log n) | O(n log n) | O(n^2) | O(log n) | no |

About my Quick Sort: the pivot is the median of the first, middle and last element. I use a 3-way partition, which deals well with duplicates. The recursion only goes into the smaller part, so the depth is at most log2 n and the stack cannot overflow.

## 4. Big array example (100 000 random integers, seed 42)

| Algorithm | Runs | Wall time (ms) | Extra memory (KB) |
|---|---:|---:|---:|
| Bubble Sort | 1 | 31,990.7 | 0.000 |
| Selection Sort | 1 | 12,641.7 | 0.000 |
| Insertion Sort | 1 | 999.1 | 0.000 |
| Merge Sort | 5 | 11.59 | 392.1 |
| Quick Sort | 5 | 11.37 | 0.800 |

I ran the three quadratic algorithms only once because they are very slow. Merge Sort and Quick Sort were run 5 times.

## 5. Comparison results

Machine: Intel Xeon 2.10 GHz (1 vCPU), 4 GB RAM, Ubuntu 24.04 (cloud VM), GCC 13.3 -O2.
`skipped` means the O(n^2) sorts are not run automatically above 50 000 elements. `*` means the value comes from a single run in the big array example.
Raw data is in `results/benchmark.csv` and the charts are in `results/chart_*.png`.

### Input: random

**Time (ms, wall-clock)**

| Algorithm | 1,000 | 5,000 | 10,000 | 50,000 | 100,000 |
|---|---:|---:|---:|---:|---:|
| Bubble Sort | 2.03 | 49.10 | 213.2 | 7,617.4 | 31,990.7\* |
| Selection Sort | 1.26 | 30.59 | 126.2 | 3,117.2 | 12,641.7\* |
| Insertion Sort | 0.140 | 2.38 | 9.37 | 241.4 | 999.1\* |
| Merge Sort | 0.052 | 0.379 | 0.761 | 5.85 | 10.17 |
| Quick Sort | 0.056 | 0.347 | 0.803 | 5.61 | 9.32 |

**Peak extra memory (KB)**

| Algorithm | 1,000 | 5,000 | 10,000 | 50,000 | 100,000 |
|---|---:|---:|---:|---:|---:|
| Bubble Sort | 0.000 | 0.000 | 0.000 | 0.000 | 0.000\* |
| Selection Sort | 0.000 | 0.000 | 0.000 | 0.000 | 0.000\* |
| Insertion Sort | 0.000 | 0.000 | 0.000 | 0.000 | 0.000\* |
| Merge Sort | 4.83 | 20.69 | 40.30 | 196.7 | 392.1 |
| Quick Sort | 0.480 | 0.550 | 0.610 | 0.730 | 0.800 |


### Input: sorted

**Time (ms, wall-clock)**

| Algorithm | 1,000 | 5,000 | 10,000 | 50,000 | 100,000 |
|---|---:|---:|---:|---:|---:|
| Bubble Sort | 0.001 | 0.004 | 0.007 | 0.038 | skipped |
| Selection Sort | 1.16 | 30.29 | 122.9 | 3,157.2 | skipped |
| Insertion Sort | 0.001 | 0.005 | 0.013 | 0.052 | skipped |
| Merge Sort | 0.017 | 0.090 | 0.192 | 1.01 | 2.10 |
| Quick Sort | 0.025 | 0.186 | 0.474 | 4.97 | 13.65 |

**Peak extra memory (KB)**

| Algorithm | 1,000 | 5,000 | 10,000 | 50,000 | 100,000 |
|---|---:|---:|---:|---:|---:|
| Bubble Sort | 0.000 | 0.000 | 0.000 | 0.000 | skipped |
| Selection Sort | 0.000 | 0.000 | 0.000 | 0.000 | skipped |
| Insertion Sort | 0.000 | 0.000 | 0.000 | 0.000 | skipped |
| Merge Sort | 4.83 | 20.69 | 40.30 | 196.7 | 392.1 |
| Quick Sort | 0.420 | 0.480 | 0.550 | 0.610 | 0.610 |


### Input: reverse

**Time (ms, wall-clock)**

| Algorithm | 1,000 | 5,000 | 10,000 | 50,000 | 100,000 |
|---|---:|---:|---:|---:|---:|
| Bubble Sort | 3.82 | 96.19 | 384.6 | 9,601.1 | skipped |
| Selection Sort | 1.16 | 30.58 | 124.9 | 3,155.5 | skipped |
| Insertion Sort | 0.197 | 4.57 | 18.73 | 461.2 | skipped |
| Merge Sort | 0.018 | 0.096 | 0.200 | 1.06 | 2.22 |
| Quick Sort | 0.019 | 0.147 | 0.366 | 3.67 | 9.95 |

**Peak extra memory (KB)**

| Algorithm | 1,000 | 5,000 | 10,000 | 50,000 | 100,000 |
|---|---:|---:|---:|---:|---:|
| Bubble Sort | 0.000 | 0.000 | 0.000 | 0.000 | skipped |
| Selection Sort | 0.000 | 0.000 | 0.000 | 0.000 | skipped |
| Insertion Sort | 0.000 | 0.000 | 0.000 | 0.000 | skipped |
| Merge Sort | 4.83 | 20.69 | 40.30 | 196.7 | 392.1 |
| Quick Sort | 0.420 | 0.480 | 0.550 | 0.610 | 0.610 |


### Input: duplicates

**Time (ms, wall-clock)**

| Algorithm | 1,000 | 5,000 | 10,000 | 50,000 | 100,000 |
|---|---:|---:|---:|---:|---:|
| Bubble Sort | 2.06 | 49.12 | 210.8 | 7,637.9 | skipped |
| Selection Sort | 1.20 | 30.76 | 125.1 | 3,138.7 | skipped |
| Insertion Sort | 0.111 | 2.43 | 9.25 | 300.0 | skipped |
| Merge Sort | 0.050 | 0.306 | 0.622 | 3.16 | 6.40 |
| Quick Sort | 0.029 | 0.161 | 0.362 | 1.51 | 3.20 |

**Peak extra memory (KB)**

| Algorithm | 1,000 | 5,000 | 10,000 | 50,000 | 100,000 |
|---|---:|---:|---:|---:|---:|
| Bubble Sort | 0.000 | 0.000 | 0.000 | 0.000 | skipped |
| Selection Sort | 0.000 | 0.000 | 0.000 | 0.000 | skipped |
| Insertion Sort | 0.000 | 0.000 | 0.000 | 0.000 | skipped |
| Merge Sort | 4.83 | 20.69 | 40.30 | 196.7 | 392.1 |
| Quick Sort | 0.300 | 0.360 | 0.300 | 0.360 | 0.360 |


![random](results/chart_random.png)
![sorted](results/chart_sorted.png)
![reverse](results/chart_reverse.png)
![duplicates](results/chart_duplicates.png)

## 6. Critical analysis

The full discussion is in the PDF report. Here is a short summary.

- For large arrays, only Quick Sort (median-of-3, 3-way) and Merge Sort are practical. They need about 2-14 ms for 100 000 elements, while the quadratic sorts need between 1 s and 32 s.
- Insertion Sort and Bubble Sort (with early exit) run in linear time on sorted data.
- Selection Sort does not care about the order of the input. It does the same work every time.
- Merge Sort needs O(n) extra memory (392 KB for 100 000 integers), but Quick Sort needs less than 1 KB.
- The measured growth is close to the theory. When n is multiplied by 5, the time grows about 25 times for O(n^2) and about 6-8 times for O(n log n). The small differences are probably caused by CPU cache effects.

Problems I faced: measuring memory per sort in C, the worst cases and recursion depth in Quick Sort, very long runs for the quadratic sorts, and making the random data reproducible.
