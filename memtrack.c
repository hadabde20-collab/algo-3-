#include "memtrack.h"
#include <stdlib.h>

#define HDR 16                       /* keeps the user block aligned */

static size_t g_cur_heap, g_peak_heap, g_peak_stack;
static char  *g_stack_base;

void *tracked_malloc(size_t size)
{
    char *raw = malloc(size + HDR);
    if (!raw) return NULL;
    *(size_t *)raw = size;
    g_cur_heap += size;
    if (g_cur_heap > g_peak_heap) g_peak_heap = g_cur_heap;
    return raw + HDR;
}

void tracked_free(void *ptr)
{
    if (!ptr) return;
    char *raw = (char *)ptr - HDR;
    g_cur_heap -= *(size_t *)raw;
    free(raw);
}

void memtrack_reset(void *stack_base)
{
    g_cur_heap = g_peak_heap = g_peak_stack = 0;
    g_stack_base = stack_base;
}

void memtrack_stack_probe(void *frame)
{
    char *p = frame;
    size_t d = (p > g_stack_base) ? (size_t)(p - g_stack_base)
                                  : (size_t)(g_stack_base - p);
    if (d > g_peak_stack) g_peak_stack = d;
}

size_t memtrack_peak_bytes(void) { return g_peak_heap + g_peak_stack; }
