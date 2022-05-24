#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define INIT_CAP 1
#define CAP_MUL 2

struct array {
	size_t isize, size, cap;
	void *data;
};

struct array *array_new(size_t isize) {
	struct array *arr = (struct array *)malloc(sizeof(struct array));
	if (arr == NULL)
		goto fail0;

	arr->isize = isize;
	arr->size = 0;
	arr->cap = INIT_CAP;
	arr->data = (void *)calloc(arr->cap, arr->isize);
	if (arr->data == NULL)
		goto fail1;

	return arr;

fail1:
	free(arr);

fail0:
	return NULL;
}

struct array *array_copy(const struct array *const arr) {
	struct array *arr_copy;

	if (arr == NULL)
		return NULL;

	arr_copy = (struct array *)malloc(sizeof(struct array));
	if (arr_copy == NULL)
		goto fail0;

	memcpy(arr_copy, arr, sizeof(struct array));

	arr_copy->data = (void *)calloc(arr_copy->cap, arr_copy->isize);
	if (arr_copy->data == NULL)
		goto fail1;

	memcpy(arr_copy->data, arr->data, arr_copy->size * arr_copy->isize);

	return arr_copy;

fail1:
	free(arr_copy);

fail0:
	return NULL;
}

void array_free(struct array *arr) {
	if (arr == NULL)
		return;

	free(arr->data);
	free(arr);
}

void *array_get(const struct array *const arr, const size_t i) {
	if (arr == NULL) return NULL;
	if (i >= arr->size) return NULL;

	return (void *)((intptr_t)arr->data + (i * arr->isize));
}

void array_set(struct array *const arr, const size_t i, const void *const data) {
	void *dest;

	if (arr == NULL) return;

	if (i >= arr->cap) {
		while (arr->cap <= i) arr->cap *= CAP_MUL;
		arr->data = (void *)reallocarray(arr->data, arr->cap, arr->isize);
	}

	if (i >= arr->size)
		arr->size = i+1;

	dest = (void *)((intptr_t)arr->data + (i * arr->isize));
	memcpy(dest, data, arr->isize);
}

void array_push(struct array *const arr, const void *const data) {
	void *dest;

	if (arr == NULL) return;

	if (arr->size == arr->cap) {
		arr->cap *= CAP_MUL;
		arr->data = (void *)reallocarray(arr->data, arr->cap, arr->isize);
	}

	dest = (void *)((intptr_t)arr->data + (arr->size * arr->isize));
	++arr->size;

	memcpy(dest, data, arr->isize);
}

void *array_pop(struct array *const arr) {
	if (arr == NULL) return NULL;
	if (arr->size < 1) return NULL;

	--arr->size;

	return (void *)((intptr_t)arr->data + (arr->size * arr->isize));
}

size_t array_get_cap(const struct array *const arr) {
	return arr != NULL ? arr->cap : 0;
}

size_t array_get_size(const struct array *const arr) {
	return arr != NULL ? arr->size : 0;
}

size_t array_get_isize(const struct array *const arr) {
	return arr != NULL ? arr->isize : 0;
}

