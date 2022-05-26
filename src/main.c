#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "settings.h"
#include "genetic.h"
#include "map.h"
#include "sdl2_app.h"

#define INDIVIDUALS	1000
#define ELITE_P		0.005
#define GENERATIONS 100000

int main(int argc, char **argv) {
	static const double agilities[CHARS] = {1.8, 1.6, 1.6, 1.6, 1.4, 0.9, 0.7};
	srandom(time(NULL));

	if (argc == 1) {

	} else if (argc == 2) {
		switch (argv[1][0]) {
			case 'g':
				gen_solve(agilities, GENERATIONS, INDIVIDUALS, ELITE_P);
				break;

			case 'a':
				sdl2_app(argc, argv);
				break;

			default:
				break;
		}
	}

	return 0;
}


