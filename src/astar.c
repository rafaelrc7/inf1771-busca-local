#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "astar.h"
#include "heap.h"

#define P(x, y, w) ((y) * (w) + x)

typedef struct _pos Pos;
struct _pos {
	size_t x, y;
};

typedef struct _node Node;
struct _node {
	Node *prev;
	size_t f, g, h, d;
	Pos pos;
	uint8_t inheap, visited;
};

struct _astar_state {
	const char *map;
	size_t width, height;
	char init_c, target_c;
	size_t (*diff)(const char);
	Pos init, target;
	Node *nodes, *current;
	Heap *heap;
};

static inline size_t manhattan(Pos a, Pos b);
static void peek(const Pos p, const Pos c, const Pos t, Heap *const heap,
				 Node *const nodes, const char *map, const size_t width,
				 size_t (*diff)(const char));


Astar *astar_init(const char *const map, const size_t width, const size_t height,
			 const char init, const char target, size_t (*diff)(const char))
{
	size_t x, y, found;
	Astar *astar;

	astar = (Astar *)malloc(sizeof(Astar));
	if (astar == NULL)
		return NULL;

	astar->map = map;
	astar->width = width;
	astar->height = height;
	astar->init_c = init;
	astar->target_c = target;
	astar->diff = diff;
	astar->current = NULL;

	astar->heap = heap_new(1);
	astar->nodes = (Node *)calloc(height * width, sizeof(Node));

	for (found = 0, y = 0; found < 2 && y < height; ++y) {
		for (x = 0; x < width; ++x) {
			if (map[P(x, y, width)] == init) {
				astar->init = (Pos){x, y};
				if (++found > 1)
					break;
			} else if (map[P(x, y, width)] == target) {
				astar->target = (Pos){x, y};
				if (++found > 1)
					break;
			}
		}
	}

	if (found < 2) {
		heap_free(astar->heap);
		free(astar->nodes);
		free(astar);
		return NULL;
	}

	x = astar->init.x;
	y = astar->init.y;

	astar->nodes[P(x, y, width)].pos = (Pos){x, y};
	astar->nodes[P(x, y, width)].prev = NULL;
	astar->nodes[P(x, y, width)].f = 0;
	astar->nodes[P(x, y, width)].g = 0;
	astar->nodes[P(x, y, width)].h = 0;
	astar->nodes[P(x, y, width)].d = 0;

	heap_push(astar->heap, 0, &astar->nodes[P(x, y, width)]);
	astar->nodes[P(x, y, width)].inheap = 1;

	return astar;
}

void astar_free(Astar *astar) {
	heap_free(astar->heap);
	free(astar->nodes);
	free(astar);
}

double astar_time(Astar *astar) {
	return astar->current->g;
}

double astar_step(Astar *astar)
{
	Node *node;
	Heap *heap = astar->heap;
	Node *nodes = astar->nodes;
	const char *map = astar->map;
	size_t (*diff)(const char) = astar->diff;
	size_t width = astar->width;
	size_t height = astar->height;
	Pos target = astar->target;


	node = (Node *)heap_pop(heap);
	astar->current = node;
	node->inheap = 0;
	size_t x = node->pos.x;
	size_t y = node->pos.y;

	nodes[P(x, y, width)].visited = 1;
	nodes[P(x, y, width)].pos = (Pos){x, y};

	if (x != target.x || y != target.y) {

		if (x > 1) {
			peek((Pos){x-1, y}, (Pos){x, y}, target, heap, nodes, map, width, diff);
		}
		if (x < width-1) {
			peek((Pos){x+1, y}, (Pos){x, y}, target, heap, nodes, map, width, diff);
		}
		if (y > 1) {
			peek((Pos){x, y-1}, (Pos){x, y}, target, heap, nodes, map, width, diff);
		}
		if (y < height-1) {
			peek((Pos){x, y+1}, (Pos){x, y}, target, heap, nodes, map, width, diff);
		}

		return 0;
	} else {
		return 1;
	}
}

double solve(const char *const map, const size_t width, const size_t height,
			 const char init, const char target, size_t (*diff)(const char))
{
	double time;
	Astar *astar = astar_init(map, width, height, init, target, diff);
	if (astar == NULL)
		return -1;

	while (!astar_step(astar));

	time = astar_time(astar);

	astar_free(astar);

	return time;
}

void astar_to_pixels(Astar *astar, uint32_t *pixels, uint32_t (*colourof)(const char)) {
	const char *map = astar->map;
	const Node *nodes = astar->nodes;
	Node *node;
	size_t x, y, c;
	size_t w = astar->width;
	size_t h = astar->height;

	for (y = 0; y < h; ++y) {
		for (x = 0; x < w; ++x) {
			c = P(x, y, w);
			if (&nodes[c] == astar->current) {
				pixels[c] = 0xFFFF0000;
			} else {
				if (nodes[c].visited) {
					/*uint8_t r = pixels[c] & 0xFF0000 >> (2 * 8);*/
					/*uint8_t g = pixels[c] & 0xFF00 >> (1 * 8);*/
					/*uint8_t b = pixels[c] & 0xFF;*/
					pixels[c] = 0xFFFFFF00;
				} else {
					pixels[c] = colourof(map[c]);
				}
			}
		}
	}

	for (node = &astar->nodes[P(astar->target.x, astar->target.y, astar->width)]; node != NULL; node = node->prev) {
		c = P(node->pos.x, node->pos.y, w);
		pixels[c] = 0xFF000000;
	}
}

static void peek(const Pos p, const Pos c, const Pos t, Heap *const heap,
				 Node *const nodes, const char *map, const size_t width,
				 size_t (*diff)(const char))
{
	Node *node;
	size_t x = p.x;
	size_t y = p.y;

	node = &nodes[P(x, y, width)];

	if (node->visited) {
		return;
	} else if (!node->inheap) {
		node->pos = (Pos){x, y};
		node->d = diff(map[P(x, y, width)]);
		node->h = manhattan(t, (Pos){x, y});

		node->prev = &nodes[P(c.x, c.y, width)];
		node->g = node->d + node->prev->g;
		node->f = node->h + node->g;

		heap_push(heap, node->f, node);
		node->inheap = 1;
	} else if (!node->visited) {
		if (heap_update(heap, &node, node->f)) {
			node->prev = &nodes[P(c.x, c.y, width)];
			node->g = node->d + node->prev->g;
			node->f = node->h + node->g;
		}
	}
}

static inline size_t manhattan(Pos a, Pos b) {
	size_t dist = 0;

	if (a.x > b.x) {
		dist += a.x - b.x;
	} else {
		dist += b.x - a.x;
	}

	if (a.y > b.y) {
		dist += a.y - b.y;
	} else {
		dist += b.y - a.y;
	}

	return dist;
}

