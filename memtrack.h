#ifndef MEMTRACK_H
#define MEMTRACK_H
#include <stddef.h>

/* Heap + stack tracking used to report the memory consumed DURING a sort.
 * - tracked_malloc/tracked_free : wrappers that record the peak heap usage.
 * - STACK_PROBE()               : records the deepest stack position reached
 *                                 by a recursive sorting function.          */

void  *tracked_malloc(size_t size);
void   tracked_free(void *ptr);
void   memtrack_reset(void *stack_base);   /* start a new measurement      */
void   memtrack_stack_probe(void *frame);
size_t memtrack_peak_bytes(void);          /* peak heap + peak stack depth */

#define STACK_PROBE() memtrack_stack_probe(__builtin_frame_address(0))
#endif
