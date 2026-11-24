CC      = gcc
CFLAGS  = -std=c11 -O2 -Wall -Wextra -pedantic
SRC     = src/main.c src/sorting.c src/benchmark.c src/utils.c src/memtrack.c

all: sorting

sorting: $(SRC) src/*.h
	$(CC) $(CFLAGS) -o $@ $(SRC)

test: tests/test_sorting.c src/sorting.c src/utils.c src/memtrack.c
	$(CC) $(CFLAGS) -o test_sorting tests/test_sorting.c src/sorting.c src/utils.c src/memtrack.c
	./test_sorting

bench: sorting
	./sorting --bench results/benchmark.csv

clean:
	rm -f sorting test_sorting
