#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "map.h"
#include "map_symbols.h"

struct _map {
	size_t width, height;
	char *cells;
};

Map *map_create_from_buff(const size_t width, const size_t height, const char *const buff) {
	Map *map;

	if (buff == NULL)
		goto fail0;

	map = (Map *)malloc(sizeof(Map));
	if (map == NULL)
		goto fail0;

	map->cells = (char *)malloc(sizeof(char) * width * height);
	if (map->cells == NULL)
		goto fail1;

	map->width = width;
	map->height = height;
	memcpy(map->cells, buff, width * height);

	return map;

fail1:
	free(map);
fail0:
	return NULL;
}

Map *map_create_from_file(const size_t width, const size_t height, FILE *f) {
	size_t h = 0;
	char *buff, *ret;
	Map *map;

	map = (Map *)malloc(sizeof(Map));
	if (map == NULL)
		goto fail0;

	map->cells = (char *)malloc(sizeof(char) * width * height);
	if (map->cells == NULL)
		goto fail1;

	map->width = width;
	map->height = height;

	buff = (char *)calloc(width + 2, sizeof(char));
	if (buff == NULL)
		goto fail2;
	while(!feof(f)) {
		ret = fgets(buff, width + 2, f);
		if (ret != NULL) {
			if (strlen(buff) != width+1)
				goto fail3;
			if (buff[width] != '\n')
				goto fail3;

			memcpy(&map->cells[h * width], buff, width);
			++h;
		}
	}
	free(buff);

	if (h != height)
		goto fail2;

	return map;

fail3:
	free(buff);
fail2:
	free(map->cells);
fail1:
	free(map);
fail0:
	return NULL;
}

void map_destroy(Map *map) {
	if (map == NULL)
		return;
	free(map->cells);
	free(map);
}

size_t map_get_width(const Map *const map) {
	return map != NULL ? map->width : 0;
}

size_t map_get_height(const Map *const map) {
	return map != NULL ? map->height : 0;
}

char map_get_cell(const Map *const map, const size_t w, const size_t h) {
	if (h >= map->height || w >= map->width)
		return 0;
	return map->cells[h * map->width + w];
}

int map_set_cell(Map *const map, const size_t w, const size_t h, const char v) {
	if (h >= map->height || w >= map->width)
		return 0;
	map->cells[h * map->width + w] = v;
	return 1;
}

const char *map_get_buff(const Map *const map) {
	return map->cells;
}

void map_print(const Map *const map) {
	size_t h;
	char *str = (char *)calloc(map->width + 1, sizeof(char));

	for (h = 0; h < map->height; ++h) {
		memcpy(str, &map->cells[h * map->width], map->width);
		printf("%s\n", str);
	}

	free(str);
}

int map_to_pixels(const Map *const map, uint32_t *pixels) {
	size_t i;

	if (map == NULL || pixels == NULL)
		return EXIT_FAILURE;

	for (i = 0; i < map->width * map->height; ++i) {
		switch (map->cells[i]) {
			case '.':
				pixels[i] = COLOURS[PLAIN];
				break;

			case 'R':
				pixels[i] = COLOURS[ROCK];
				break;

			case 'F':
				pixels[i] = COLOURS[FOREST];
				break;

			case 'A':
				pixels[i] = COLOURS[WATER];
				break;

			case 'M':
				pixels[i] = COLOURS[MOUNTAIN];
				break;

			default:
				pixels[i] = COLOURS[OBJECTIVE];
				break;
		}
	}

	return EXIT_SUCCESS;
}

uint32_t map_cell_colour(const char c) {

		switch (c) {
			case '.':	return COLOURS[PLAIN];
			case 'R': 	return COLOURS[ROCK];
			case 'F': 	return COLOURS[FOREST];
			case 'A': 	return COLOURS[WATER];
			case 'M': 	return COLOURS[MOUNTAIN];
			default:	return COLOURS[OBJECTIVE];
		}
}

