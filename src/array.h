#ifndef _ARRAY_H
#define _ARRAY_H

#include <stdlib.h>

struct array;

struct array *array_new(size_t isize);
struct array *array_copy(struct array *arr);
void array_free(struct array *arr);
void *array_get(struct array *arr, size_t i);
void array_set(struct array *arr, size_t i, void *data);
void array_push(struct array *arr, void *data);
void *array_pop(struct array *arr);
ssize_t array_get_cap(struct array *arr);
ssize_t array_get_size(struct array *arr);
ssize_t array_get_isize(struct array *arr);

#endif

