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

int main(int argc, char **argv) {
	static const double agilities[CHARS] = {1.8, 1.6, 1.6, 1.6, 1.4, 0.9, 0.7};
	srandom(time(NULL));

	if (argc == 1) {
		usage(argv[0]);
		return EXIT_FAILURE;
	} else {
		switch (argv[1][0]) {
			case 'g':
				gen_solve(agilities, GENERATIONS, POP_STEP, POP_CAP);
				break;

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

	return EXIT_SUCCESS;
}

static void usage(const char *const prog) {
	fprintf(stderr, "USAGE: %1$s [g,a,m]\n"
					"       %1$s a <file>\n", prog);
}

