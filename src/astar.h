#ifndef _ASTAR_H
#define _ASTAR_H

#include <stddef.h>
#include <stdint.h>

typedef struct _astar_state Astar;

double solve(const char *const map, const size_t width, const size_t height,
			 const char init, const char target, size_t (*diff)(const char));
Astar *astar_init(const char *const map, const size_t width, const size_t height,
			 const char init, const char target, size_t (*diff)(const char));
void astar_free(Astar *astar);
double astar_step(Astar *astar);
double astar_time(Astar *astar);
void astar_to_pixels(Astar *astar, uint32_t *pixels, uint32_t (*colourof)(const char));
void astar_markpath(Astar *astar, uint32_t *path);

#endif

