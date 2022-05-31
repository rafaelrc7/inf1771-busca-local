#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

int main(int argc, char **argv) {
	static const double agilities[CHARS] = {1.8, 1.6, 1.6, 1.6, 1.4, 0.9, 0.7};
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
			case 'g': {
				double result;
				result = gen_solve(agilities, GENERATIONS, POP_STEP, POP_CAP);
				printf("\n\tTIME: %.020f\n", result);
				break;
			}

			case 'a':
				sdl2_app(argc, argv);
				break;

			case 'm': {
				Map *map = map_create_from_file(300, 82, stdin);
				map_print(map);
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
	fprintf(stderr, "USAGE: %1$s [g,a,m]\n"
					"       %1$s a <file>\n", prog);
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

