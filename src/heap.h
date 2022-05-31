#ifndef _HEAP_H
#define _HEAP_H

#include <stddef.h>

typedef struct _heap Heap;

Heap *heap_new(const size_t max);
void heap_free(Heap *const heap);
void heap_push(Heap *restrict const heap, const unsigned int priority, void *restrict const data);
void *heap_pop(Heap *const heap);
void heap_update(Heap *restrict const heap, const void *restrict const data, const unsigned int priority);
void heap_print(const Heap *const heap);

#endif

