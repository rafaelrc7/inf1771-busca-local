#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "heap.h"

#define FATHER(i) ((((size_t)(i))-1)/2)
#define LEFSON(i) ((size_t)(i)*2 +1)
#define RIGSON(i) ((size_t)(i)*2 +2)

typedef struct _node Node;
struct _node {
	void *data;
	size_t priority;
};

struct _heap {
	Node *nodes;
	size_t cap, size;
};

static void heap_swap(Heap *heap, size_t i, size_t j);
static void heap_correct_up(Heap *heap, size_t index);
static void heap_correct_down(Heap *heap, size_t index);
static inline size_t next_pow2(const size_t n);

Heap *heap_new(const size_t max) {
	Heap *heap = (Heap *)malloc(sizeof(Heap));
	if (heap == NULL)
		goto fail0;

	heap->cap = next_pow2(max);
	heap->size = 0;

	heap->nodes = (Node *)calloc(heap->cap, sizeof(Node));
	if (heap->nodes == NULL)
		goto fail1;

	return heap;

fail1:
	free(heap);
fail0:
	return NULL;
}

void heap_free(Heap *const heap) {
	free(heap->nodes);
	free(heap);
}

void heap_push(Heap *restrict const heap,
			   const size_t priority,
			   void *restrict const data)
{
	if (heap->size == heap->cap) {
		heap->cap *= 2;
		heap->nodes = (Node *)reallocarray(heap->nodes, heap->cap, sizeof(Node));
	}

	heap->nodes[heap->size].data = data;
	heap->nodes[heap->size].priority = priority;
	heap_correct_up(heap, heap->size);
	++heap->size;
}

void *heap_pop(Heap *const heap) {
	void *data;
	if (heap->size == 0)
		return NULL;

	data = heap->nodes[0].data;

	if (--heap->size > 0) {
		heap->nodes[0] = heap->nodes[heap->size];
		heap_correct_down(heap, 0);
	}

	return data;
}

int heap_update(Heap *restrict const heap,
				  const void *restrict const data,
				  const size_t priority)
{
	size_t i;
	for (i = 0; i < heap->size; ++i) {
		if (heap->nodes[i].data == data) {
			if (heap->nodes[i].priority > priority) {
				heap->nodes[i].priority = priority;
				heap_correct_up(heap, i);
				return 1;
			}
			return 0;
		}
	}
	return 0;
}

static void heap_correct_up(Heap *heap, size_t index) {
	size_t father;
	while (index > 0) {
		father = FATHER(index);
		if (heap->nodes[father].priority > heap->nodes[index].priority)
			heap_swap(heap, father, index);
		else
			break;

		index = father;
	}
}

static void heap_correct_down(Heap *heap, size_t index) {
	size_t lson, rson, son;
	while ((lson = LEFSON(index)) < heap->size) {
		rson = RIGSON(index);
		if (rson < heap->size) {
			son = heap->nodes[lson].priority < heap->nodes[rson].priority ?
					lson : rson;
		} else {
			son = lson;
		}

		if (heap->nodes[index].priority > heap->nodes[son].priority)
			heap_swap(heap, index, son);
		else
			break;

		index = son;
	}
}

static void heap_swap(Heap *heap, size_t i, size_t j) {
	Node tmp = heap->nodes[i];
	heap->nodes[i] = heap->nodes[j];
	heap->nodes[j] = tmp;
}

static inline size_t next_pow2(size_t x) {
	return x == 1 ? 1 : 1 << (64 - __builtin_clzl(x-1));
}

void heap_print(const Heap *const heap) {
	size_t i;
	printf("Size: %lu\tCap: %lu\n", heap->size, heap->cap);
	putchar('[');
	for (i = 0; i < heap->size; ++i) {
		printf("  %lu", heap->nodes[i].priority);
	}
	puts("  ]");
}

