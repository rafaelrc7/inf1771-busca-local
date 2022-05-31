#ifndef _MAP_SYMBOLS_H
#define _MAP_SYMBOLS_H

#include <stdint.h>

#define CELL_TYPE_BITS 3
enum CELL_TYPE { PLAIN, ROCK, FOREST, WATER, MOUNTAIN, OBJECTIVE, };

static const uint32_t COLOURS[] = {
	0xFF657A2F, /* PLAIN     */
	0xFF33322F, /* ROCK      */
	0xFF283012, /* FOREST    */
	0xFF0014A8, /* WATER     */
	0xFF2D2008, /* MOUNTAIN  */
	0xFFFFFFFF, /* OBJECTIVE */
};

enum CELL_STAT {
	CHAR	= (1 << CELL_TYPE_BITS),
	MARKED	= (2 << CELL_TYPE_BITS),
	PATH	= (4 << CELL_TYPE_BITS),
};

#endif

