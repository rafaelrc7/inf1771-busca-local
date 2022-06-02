#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "settings.h"

#define DEFAULT_STAGES		31
#define DEFAULT_CHARS		7
#define DEFAULT_MWIDTH		300
#define DEFAULT_MHEIGHT		82
#define DEFAULT_POP_CAP		1000
#define DEFAULT_POP_STEP	300
#define DEFAULT_GENERATIONS 2100

static int lua_get_size_t(lua_State *L, size_t *const dest, const char *const field_name);
static void lua_print_err(lua_State *L);
static void print_error(const char *msg);

Settings *settings_new(void) {
	Settings *s;

	s =  (Settings *)calloc(1, sizeof(Settings));
	if (s != NULL) {
		s->char_num = DEFAULT_CHARS;
		s->generation_num = DEFAULT_GENERATIONS;
		s->population_cap = DEFAULT_POP_CAP;
		s->population_step = DEFAULT_POP_STEP;
		s->stage_num = DEFAULT_STAGES;
		s->map_width = DEFAULT_MWIDTH;
		s->map_height = DEFAULT_MHEIGHT;
	}

	return s;
}

Settings *settings_from_lua(const char *filename) {
	lua_State *L;
	Settings *s;

	if ((s = settings_new()) == NULL)
		goto fail0;

	if ((L = luaL_newstate()) == NULL)
		goto fail1;

	if (luaL_dofile(L, filename)) {
		lua_print_err(L);
		goto fail2;
	}

	if (!lua_istable(L, -1)) {
		print_error("Expected table from script");
		goto fail2;
	}

	if (lua_getfield(L, -1, "seed") != LUA_TNIL) {
		if (!lua_isnumber(L, -1)) {
			print_error("'seed' field is not number");
			goto fail2;
		}
		s->seed = lua_tonumber(L, -1);
		s->use_seed = 1;
	}
	lua_pop(L, 1);

	if (lua_getfield(L, -1, "win_width_scale") != LUA_TNIL) {
		if (!lua_isnumber(L, -1)) {
			print_error("'win_width_scale' field is not number");
			goto fail2;
		}
		s->size_is_scale = 1;
		s->win_width.scale = lua_tonumber(L, -1);
	}
	lua_pop(L, 1);

	if (lua_getfield(L, -1, "win_height_scale") != LUA_TNIL) {
		if (!lua_isnumber(L, -1)) {
			print_error("'win_height_scale' field is not number");
			goto fail2;
		}
		s->size_is_scale = 1;
		s->win_height.scale = lua_tonumber(L, -1);
	}
	lua_pop(L, 1);

	if (!s->size_is_scale) {
		if (lua_get_size_t(L, &s->win_height.val, "win_height"))
			goto fail2;

		if (lua_get_size_t(L, &s->win_width.val, "win_width"))
			goto fail2;
	}


	if (lua_get_size_t(L, &s->char_num, "char_num"))
		goto fail2;

	if (lua_get_size_t(L, &s->generation_num, "generation_num"))
		goto fail2;

	if (lua_get_size_t(L, &s->population_cap, "population_cap"))
		goto fail2;

	if (lua_get_size_t(L, &s->population_step, "population_step"))
		goto fail2;

	if (lua_get_size_t(L, &s->stage_num, "stage_num"))
		goto fail2;

	if (lua_get_size_t(L, &s->map_width, "map_width"))
		goto fail2;

	if (lua_get_size_t(L, &s->map_height, "map_height"))
		goto fail2;

	if (lua_getfield(L, -1, "app_name") != LUA_TNIL) {
		const char *tmp;

		if (!lua_isstring(L, -1)) {
			print_error("'app_name' field is not string");
			goto fail2;
		}

		tmp = lua_tostring(L, -1);

		lua_len(L, -1);
		s->app_name_len = lua_tonumber(L, -1);

		if ((s->app_name = (char *)calloc(s->app_name_len, sizeof(char))) == NULL) {
			print_error("memory allocation error");
			goto fail2;
		}

		strncpy(s->app_name, tmp, s->app_name_len);

		lua_pop(L, 1);
	}
	lua_pop(L, 1);

	if (lua_getfield(L, -1, "waypoints") != LUA_TNIL) {
		size_t i;

		if (!lua_istable(L, -1)) {
			print_error("'waypoints' field is not table");
			goto fail2;
		}

		lua_len(L, -1);
		s->waypoint_num = lua_tonumber(L, -1);
		lua_pop(L, 1);

		if ((s->waypoints = (char *)calloc(s->waypoint_num, sizeof(char))) == NULL) {
			print_error("memory allocation error");
			goto fail2;
		}

		for (i = 1; i <= s->waypoint_num; ++i) {
			const char *tmp;
			lua_rawgeti(L, -1, i);

			if (!lua_isstring(L, -1)) {
				print_error("waypoints element is not string");
				goto fail2;
			}

			lua_len(L, -1);
			if (lua_tonumber(L, -1) > 1) {
				print_error("waypoints element has more than 1 character");
				goto fail2;
			}

			tmp = lua_tostring(L, -2);
			s->waypoints[i-1] = tmp[0];

			lua_pop(L, 2);
		}

	}
	lua_pop(L, 1);

	if (lua_getfield(L, -1, "agilities") != LUA_TNIL) {
		size_t i;

		if (!lua_istable(L, -1)) {
			print_error("'agilities' field is not table");
			goto fail2;
		}

		lua_len(L, -1);
		if (lua_tonumber(L, -1) != s->char_num) {
			print_error("number of agility elements differs from number of characters");
			goto fail2;
		}
		lua_pop(L, 1);

		if ((s->agilities = (double *)calloc(s->char_num, sizeof(double))) == NULL) {
			print_error("memory allocation error");
			goto fail2;
		}

		for (i = 1; i <= s->char_num; ++i) {
			lua_rawgeti(L, -1, i);

			if (!lua_isnumber(L, -1)) {
				print_error("waypoints element is not number");
				goto fail2;
			}

			s->agilities[i-1] = lua_tonumber(L, -1);

			lua_pop(L, 1);
		}

	}
	lua_pop(L, 1);

	lua_close(L);

	return s;

fail2:
	lua_close(L);

fail1:
	settings_free(s);

fail0:
	return NULL;
}

void settings_free(Settings *s) {
	if (s->agilities)
		free(s->agilities);

	if (s->app_name)
		free(s->app_name);

	if (s->waypoints)
		free(s->waypoints);

	free(s);
}

void settings_print(const Settings *const s) {
	size_t i;

	puts("------- SETTINGS -------");
	printf(	"APP NAME: %s\n"
			"SEED: %d\n"
			"MAP WIDTH: %lu\n"
			"MAP HEIGHT: %lu\n"
			"STAGE NUM: %lu\n"
			"CHAR NUM: %lu\n"
			"GENERATION NUM: %lu\n"
			"POPULATION CAP: %lu\n"
			"POPULATION STEP: %lu\n",
		s->app_name, s->seed, s->map_width, s->map_height, s->stage_num,
		s->char_num, s->generation_num, s->population_cap, s->population_step);

	if (s->size_is_scale) {
		printf(	"WIN WIDTH SCALE: %.02f\n"
				"WIN HEIGHT SCALE: %.02f\n",
		 s->win_width.scale, s->win_height.scale);
	} else {
		printf(	"WIN WIDTH: %lu\n"
				"WIN HEIGHT: %lu\n",
		 s->win_width.val, s->win_height.val);
	}

	printf("AGILITIES: {");
	for (i = 0; i < s->char_num; ++i) {
		printf(" %.2f", s->agilities[i]);
	}
	puts(" }");

	printf("WAYPOINTS: {");
	for (i = 0; i < s->waypoint_num; ++i) {
		printf(" '%c'", s->waypoints[i]);
	}
	puts(" }");

	puts("------------------------");
}

static int lua_get_size_t(lua_State *L, size_t *const dest, const char *const field_name) {
	if (lua_getfield(L, -1, field_name) != LUA_TNIL) {
		if (!lua_isnumber(L, -1)) {
			fprintf(stderr, "LUA ERROR: '%s' field is not number\n", field_name);
			lua_pop(L, 1);
			return 1;
		}
		*dest = lua_tonumber(L, -1);
	}
	lua_pop(L, 1);
	return 0;
}

static void lua_print_err(lua_State *L) {
	const char *err;
	if (!lua_isstring(L, -1))
		goto leave;

	err = lua_tostring(L, -1);
	print_error(err);

leave:
	lua_pop(L, 1);
}

static void print_error(const char *msg) {
	fprintf(stderr, "LUA ERROR: %s\n", msg);
}

