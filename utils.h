#ifndef UTILS_H
#define UTILS_H
#include <stddef.h>

typedef enum { INPUT_RANDOM, INPUT_SORTED, INPUT_REVERSE, INPUT_DUPLICATES,
               INPUT_TYPES } InputType;

const char *input_type_name(InputType t);
void  generate_array(int *a, size_t n, InputType t, unsigned long long seed);
void  copy_array(int *dst, const int *src, size_t n);
int   is_sorted(const int *a, size_t n);
void  print_array(const char *label, const int *a, size_t n);
#endif
