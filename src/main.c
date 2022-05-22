#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define STAGES	31
#define CHARS	7

#define GENEMASK 0xFF

#define STARTPOOL	1000
#define SONS		1000
#define KEEP		3
#define GENERATIONS 100000

struct solution {
	uint8_t genes[STAGES];
	double time;
};
typedef struct solution Solution;

static int validate_genes(const uint8_t genes[STAGES]);
static int generate_genes(Solution *solutions, const size_t size, const double agilities[CHARS]);
static void print_solutions(const Solution *const solutions, const size_t size, const size_t cap);
static double time_stage(const uint8_t gene, const double agilities[CHARS], const uint8_t stage);
static double time_stages(const uint8_t genes[STAGES], const double agilities[CHARS]);
static int solution_sort(const void *ptr1, const void *ptr2);
static size_t roulette(const size_t size);

int main(void) {
	static const double agilities[CHARS] = {1.8, 1.6, 1.6, 1.6, 1.4, 0.9, 0.7};
	Solution *solutions = (Solution *)calloc(STARTPOOL, sizeof(Solution));
	size_t g, individual_count;

	srandom(time(NULL));

	generate_genes(solutions, STARTPOOL, agilities);
	individual_count = STARTPOOL;

	for (g = 0; g < GENERATIONS; ++g) {
		Solution *new_solutions;
		size_t i;
		qsort(solutions, individual_count, sizeof(Solution), &solution_sort);

		printf("\n----- GENERATION %lu -----\n", g);
		print_solutions(solutions, individual_count, 5);

		new_solutions = (Solution *)calloc(SONS + KEEP, sizeof(Solution));
		for (i = 0; i < KEEP; ++i) {
			new_solutions[i] = solutions[i];
		}

		for (; i < SONS + KEEP;) {
			size_t j, mut;
			size_t parent1 = random() % (size_t)(0.4 * individual_count);
			size_t parent2 = random() % (size_t)(0.4 * individual_count);

			for (j = 0; j < STAGES; ++j) {
				uint8_t select = random() % 2;
				new_solutions[i].genes[j] = select ? solutions[parent1].genes[j] : solutions[parent2].genes[j];
			}

			mut = random() % 100;

			if (mut < 20) {
				size_t id1 = random() % STAGES;
				size_t id2 = random() % STAGES;
				uint8_t tmp = new_solutions[i].genes[id2];
				new_solutions[i].genes[id2] = new_solutions[i].genes[id1];
				new_solutions[i].genes[id1] = tmp;
			} else if (mut < 30) {
				size_t id1 = random() % STAGES;
				size_t id2 = random() % STAGES;
				uint8_t mask = 1 << (random() % CHARS);

				uint8_t tmp = new_solutions[i].genes[id2] & mask;
				new_solutions[i].genes[id2] = (new_solutions[i].genes[id2] & ~mask) | (new_solutions[i].genes[id1] & mask);
				new_solutions[i].genes[id1] = (new_solutions[i].genes[id1] & ~mask) | tmp;
			} else if (mut < 50) {
				uint8_t mask = 1 << (random() % CHARS);
				new_solutions[i].genes[random() % STAGES] ^= mask;
			}

			if (validate_genes(new_solutions[i].genes)) {
				new_solutions[i].time = time_stages(new_solutions[i].genes, agilities);
				++i;
			}
		}

		free(solutions);
		solutions = new_solutions;

		individual_count = SONS + KEEP;
	}

	free(solutions);

	return 0;
}

static int validate_genes(const uint8_t genes[STAGES]) {
	size_t i, j;
	uint8_t chars[CHARS];
	memset(chars, 0, sizeof(chars));

	for (i = 0; i < STAGES; ++i) {
		uint8_t mask = 1;
		if (!genes[i])
			return 0;

		for (j = 0; j < CHARS; ++j, mask <<= 1) {
			if (mask & genes[i]) {
				if (++chars[j] > 8)
					return 0;
			}
		}
	}

	return 1;
}

static int generate_genes(Solution *solutions, const size_t size, const double agilities[CHARS]) {
	size_t i, j;

	for (i = 0; i < size;) {
		for (j = 0; j < STAGES; ++j) {
			solutions[i].genes[j] = 1 << random() % CHARS | 1 << random() % CHARS;
		}

		if (validate_genes(solutions[i].genes)) {
			solutions[i].time = time_stages(solutions[i].genes, agilities);
			++i;
		}
	}

	return 1;
}

static void print_solutions(const Solution *const solutions, const size_t size, const size_t cap) {
	size_t i, j, k;
	char buff[CHARS + 1];

	for (i = 0; i < cap && i < size; ++i) {
		for (j = 0; j < STAGES; ++j) {
			uint8_t mask = 1 << (CHARS - 1);
			for (k = 0; k < CHARS; ++k, mask >>= 1) {
				buff[k] = solutions[i].genes[j] & mask ? '1' : '0';
			}
			buff[k] = 0;
			printf("%s ", buff);
		}
		printf("\t%.05f\n", solutions[i].time);
	}
}

static double time_stage(const uint8_t gene, const double agilities[CHARS], const uint8_t stage) {
	size_t i;
	uint8_t mask;
	double diff = (stage+1) * 10;
	double ag_sum = 0;

	for (i = 0, mask = 1; i < CHARS; ++i, mask <<= 1) {
		if (gene & mask)
			ag_sum += agilities[i];
	}

	return diff / ag_sum;
}

static double time_stages(const uint8_t genes[STAGES], const double agilities[CHARS]) {
	size_t i;
	double time = 0;

	for (i = 0; i < STAGES; ++i) {
		time += time_stage(genes[i], agilities, i);
	}

	return time;
}

static int solution_sort(const void *ptr1, const void *ptr2) {
	double diff = ((Solution *)ptr1)->time - ((Solution *)ptr2)->time;

	if (diff < 0)
		return -1;
	else if (diff == 0)
		return 0;
	else
		return 1;
}

static size_t roulette(const size_t size) {
	size_t i;
	double r = random() / (double)RAND_MAX;

	for (i = 0; i < size; ++i) {
		double pi = (size - i) / (double)size;
		if (r < pi) {
			return i;
		} else {
			r -= pi;
		}
	}

	return -1;
}

