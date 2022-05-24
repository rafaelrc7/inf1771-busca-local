#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "settings.h"
#include "genetic.h"

#define INDIVIDUALS	1000
#define ELITE		3
#define GENERATIONS 100000

int main(void) {
	static const double agilities[CHARS] = {1.8, 1.6, 1.6, 1.6, 1.4, 0.9, 0.7};
	srandom(time(NULL));

	gen_solve(agilities, GENERATIONS, INDIVIDUALS, ELITE);

	return 0;
}


