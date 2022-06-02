#ifndef _SETTINGS_H
#define _SETTINGS_H

#include <stddef.h>
#include <stdint.h>

#define STAGES		31
#define CHARS		7
#define D_WIDTH		300
#define D_HEIGHT	82

typedef struct _settings Settings;
struct _settings {
	char *waypoints, *app_name;
	double *agilities;
	size_t map_width, map_height;
	size_t waypoint_num;
	size_t app_name_len;
	size_t stage_num, char_num;
	size_t generation_num, population_cap, population_step;
	union {
		size_t val;
		double scale;
	} win_width;
	union {
		size_t val;
		double scale;
	} win_height;
	int seed;
	uint8_t size_is_scale, use_seed;
};

Settings *settings_new(void);
Settings *settings_from_lua(const char *file);
void settings_free(Settings *s);
void settings_print(const Settings *const s);

#endif

