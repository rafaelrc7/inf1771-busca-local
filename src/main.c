#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "astar.h"
#include "settings.h"
#include "genetic.h"
#include "map.h"
#include "sdl2_app.h"

#define LUA_CONFIG_FILE "config.lua"

static void usage(const char *const prog);
static void die(const char *const msg);
static int parsei(const char *str, int *const num);
static int parseul(const char *str, unsigned long *const num);
static size_t diff(const char c);

int main(int argc, char **argv) {
	Settings *s;

	char *seed_env = getenv("SEED");

	s = settings_from_lua(LUA_CONFIG_FILE);
	if (!s) {
		fprintf(stderr, "FATAL: Settings failure\n");
		return EXIT_FAILURE;
	}

	if (!s->use_seed) {
		if (seed_env == NULL) {
			size_t r;
			FILE *urandom = fopen("/dev/urandom", "r");
			if (urandom == NULL) {
				die("fopen()");
			}
			r = fread(&s->seed, 1, sizeof(s->seed), urandom);
			fclose(urandom);
			if (r != sizeof(s->seed))
				die("fread()");
		} else {
			int ret;
			ret = parsei(seed_env, &s->seed);
			if (ret != EXIT_SUCCESS)
				die("strtoi");
		}
	}

	srandom(s->seed);

	if (argc == 1) {
		usage(argv[0]);
		return EXIT_FAILURE;
	} else {
		switch (argv[1][0]) {
			case 'a':
				sdl2_app(s);
				break;

			case 'g': {
				double result;
				result = gen_solve(s);
				printf("\n\tTIME: %.020f\n", result);
				break;
			}

			case 'h': {
				size_t i;
				double lc, r, as = 0;
				Map *map = map_create_from_file(s->map_width, s->map_height, stdin);

				lc = gen_solve(s);
				printf("\n\tLOCAL SEARCH TIME: %.020f\n", lc);

				puts("\nA*:");
				for (i = 0; i < s->waypoint_num-1; ++i) {
					r = solve(map_get_buff(map), map_get_width(map), map_get_height(map), s->waypoints[i], s->waypoints[i+1], &diff);
					printf("%lu = %.0f\n", i, r);
					as += r;
				}
				printf("\nTOTAL A* = %.0f\n", as);
				map_destroy(map);

				printf("\n\t TOTAL TIME = %f\n", (as + lc));
				break;
			}

			case 'm': {
				size_t i;
				double r, t = 0;
				Map *map = map_create_from_file(s->map_width, s->map_height, stdin);
				for (i = 0; i < s->waypoint_num-1; ++i) {
					r = solve(map_get_buff(map), map_get_width(map), map_get_height(map), s->waypoints[i], s->waypoints[i+1], &diff);
					printf("%lu = %f\n", i, r);
					t += r;
				}
				printf("\nTOTAL A* = %f\n", t);
				map_destroy(map);
				break;
			}

			default:
				usage(argv[0]);
				return EXIT_FAILURE;
		}
	}

	printf("\n\tSEED: %d\n", s->seed);

	settings_free(s);
	return EXIT_SUCCESS;
}

static void usage(const char *const prog) {
	fprintf(stderr, "USAGE: %1$s [g,a,m,h]\n"
					"       %1$s a < <file>\n"
					"       %1$s h < <file>\n",
					prog);
}

static int parsei(const char *str, int *const num)
{
	char *endptr;

	if (str == NULL || *str == 0 || num == NULL)
		return EXIT_FAILURE;

	*num = (int)strtol(str, &endptr, 0);
	if (*endptr != 0)
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}

static int parseul(const char *str, unsigned long *const num) {
	char *endptr;

	if (str == NULL || *str == 0 || num == NULL)
		return EXIT_FAILURE;

	*num = strtoul(str, &endptr, 0);
	if (*endptr != 0)
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}

static void die(const char *const msg) {
	perror(msg);
	exit(EXIT_FAILURE);
}

static size_t diff(const char c) {
	switch (c) {
		case '.':
			return 1;

		case 'R':
			return 5;

		case 'F':
			return 10;

		case 'A':
			return 15;

		case 'M':
			return 200;

		default:
			return 0;
			break;
	}
}

