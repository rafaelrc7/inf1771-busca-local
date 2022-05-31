#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "astar.h"
#include "settings.h"
#include "genetic.h"
#include "map.h"
#include "sdl2_app.h"

#define POP_CAP		1000
#define POP_STEP	300
#define GENERATIONS 2100

static void usage(const char *const prog);
static void die(const char *const msg);
static int strtoi(const char *str, int *const num);
static size_t diff(const char c);

int main(int argc, char **argv) {
	static const double agilities[CHARS] = {1.8, 1.6, 1.6, 1.6, 1.4, 0.9, 0.7};
	static const char waypoints[STAGES+1] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'B', 'C', 'D', 'E', 'G', 'H', 'I', 'J', 'K', 'L', 'N', 'O', 'P', 'Q', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
	int seed;

	char *seed_env = getenv("SEED");

	if (seed_env == NULL) {
		FILE *urandom = fopen("/dev/urandom", "r");
		if (urandom == NULL) {
			die("fopen()");
		}
		fread(&seed, 1, sizeof(seed), urandom);
		fclose(urandom);
	} else {
		int ret;
		ret = strtoi(seed_env, &seed);
		if (ret != EXIT_SUCCESS)
			die("strtoi");
	}

	srandom(seed);


	if (argc == 1) {
		usage(argv[0]);
		return EXIT_FAILURE;
	} else {
		switch (argv[1][0]) {
			case 'a':
				sdl2_app(argc, argv);
				break;

			case 'g': {
				double result;
				result = gen_solve(agilities, GENERATIONS, POP_STEP, POP_CAP);
				printf("\n\tTIME: %.020f\n", result);
				break;
			}

			case 'h': {
				size_t i;
				double lc, r, as = 0;
				Map *map = map_create_from_file(300, 82, stdin);

				lc = gen_solve(agilities, GENERATIONS, POP_STEP, POP_CAP);
				printf("\n\tLOCAL SEARCH TIME: %.020f\n", lc);

				puts("\nA*:");
				for (i = 0; i < sizeof(waypoints)-1; ++i) {
					r = solve(map_get_buff(map), map_get_width(map), map_get_height(map), waypoints[i], waypoints[i+1], &diff);
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
				Map *map = map_create_from_file(300, 82, stdin);
				for (i = 0; i < sizeof(waypoints)-1; ++i) {
					r = solve(map_get_buff(map), map_get_width(map), map_get_height(map), waypoints[i], waypoints[i+1], &diff);
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

	printf("\n\tSEED: %d\n", seed);
	return EXIT_SUCCESS;
}

static void usage(const char *const prog) {
	fprintf(stderr, "USAGE: %1$s [g,a,m,h]\n"
					"       %1$s a < <file>\n"
					"       %1$s h < <file>\n",
					prog);
}

static int strtoi(const char *str, int *const num)
{
	char *endptr;

	if (str == NULL || *str == 0 || num == NULL)
		return EXIT_FAILURE;

	*num = (int)strtol(str, &endptr, 0);
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

