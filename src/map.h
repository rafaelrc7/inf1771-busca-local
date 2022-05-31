#ifndef _MAP_H
#define _MAP_H

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>

typedef struct _map Map;

Map *map_create_from_buff(const size_t width, const size_t height, const char *const buff);
Map *map_create_from_file(const size_t width, const size_t height, FILE *f);
void map_destroy(Map *map);

size_t map_get_width(const Map *const map);
size_t map_get_height(const Map *const map);

char map_get_cell(const Map *const map, const size_t w, const size_t h);
int map_set_cell(Map *const map, const size_t w, const size_t h, const char v);

const char *map_get_buff(const Map *const map);

void map_print(const Map *const map);
int map_to_pixels(const Map *const map, uint32_t *pixels);
uint32_t map_cell_colour(const char c);

#endif

