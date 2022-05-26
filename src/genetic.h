#ifndef _GENETIC_H
#define _GENETIC_H

#include <stddef.h>

#include "settings.h"

double gen_solve(const double agilities[CHARS], const size_t generation_num,
			 const size_t individual_num, const double elite_percent);

#endif

