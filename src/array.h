#ifndef _ARRAY_H
#define _ARRAY_H

#include <stdlib.h>

struct array;

struct array *array_new(size_t isize);
struct array *array_copy(const struct array *const arr);
void array_free(struct array *arr);
void *array_get(const struct array *const arr, const size_t i);
void array_set(struct array *const arr, const size_t i, const void *const data);
void array_push(struct array *const arr, const void *const data);
void *array_pop(struct array *const arr);
size_t array_get_cap(const struct array *const arr);
size_t array_get_size(const struct array *const arr);
size_t array_get_isize(const struct array *const arr);

#endif

