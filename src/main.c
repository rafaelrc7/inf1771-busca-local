#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "settings.h"
#include "genetic.h"
#include "sdl2_app.h"

#define INDIVIDUALS	1000
#define ELITE_P		0.005
#define GENERATIONS 100000

int main(int argc, char **argv) {
	static const double agilities[CHARS] = {1.8, 1.6, 1.6, 1.6, 1.4, 0.9, 0.7};
	srandom(time(NULL));

	/*sdl2_app(argc, argv);*/
	gen_solve(agilities, GENERATIONS, INDIVIDUALS, ELITE_P);

	return 0;
}


