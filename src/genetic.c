#include <float.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "genetic.h"

#include "settings.h"
#include "array.h"
#include "random_utils.h"

#define PRINT_GENS

#define INDV_STEP	((size_t)500)
#define	INDV_CAP	((size_t)2000)

struct solution {
	uint8_t genes[STAGES];
	double time;
	double fitness;
};
typedef struct solution Solution;

static int validate_genes(const uint8_t genes[STAGES]);
static int generate_genes(Solution *solutions, const size_t size, const double agilities[CHARS]);
static void print_solutions(const Solution *const solutions, const size_t size, const size_t cap);
static double time_stages(const uint8_t genes[STAGES], const double agilities[CHARS]);
static size_t roulette(const Solution *const solutions, const size_t solutions_num, const double total_fitness);
static double total_fitness(const Solution *const solutions, size_t solutions_num);
static double fitness(const Solution solution);
static size_t cut_solutions(Solution *const solutions, const size_t solutions_cap, const size_t solutions_num);
int solution_sort(const void *ptr1, const void *ptr2);

double gen_solve(const double agilities[CHARS], const size_t generation_num,
			 const size_t _individual_num, const double elite_percent)
{
	Solution *solutions;
	size_t generation, solutions_num;
	double time;

	solutions = (Solution *)calloc(INDV_CAP + INDV_STEP, sizeof(Solution));

	generate_genes(solutions, INDV_STEP, agilities);
	solutions_num = INDV_STEP;

	qsort(solutions, solutions_num, sizeof(Solution), &solution_sort);

	for (generation = 0; generation < generation_num; ++generation) {
		size_t i, j;
		double total_fitness_val = total_fitness(solutions, solutions_num);

		#ifdef PRINT_GENS
		printf("\n----- GENERATION %lu (%lu/%lu) -----\n", generation, solutions_num, INDV_CAP);
		print_solutions(solutions, solutions_num, 5);
		#endif

		for (i = solutions_num; i < solutions_num + INDV_STEP;) {
			size_t parent1 = roulette(solutions, solutions_num, total_fitness_val);
			size_t parent2 = roulette(solutions, solutions_num, total_fitness_val);
			uint8_t mut;

			for (j = 0; j < STAGES; ++j) {
				uint8_t select = random_prob(2);
				solutions[i].genes[j] = select ?
					solutions[parent1].genes[j] : solutions[parent2].genes[j];
			}

			mut = random_lim(100);
			if (mut < 50) {
				size_t id1 = random_lim(STAGES);
				size_t id2 = random_lim(STAGES);

				uint8_t tmp = solutions[i].genes[id2];
				solutions[i].genes[id2] = solutions[i].genes[id1];
				solutions[i].genes[id1] = tmp;
			}

			mut = random_lim(100);
			if (mut < 50) {
				size_t id1 = random_lim(STAGES);
				size_t id2 = random_lim(STAGES);
				uint8_t mask = 1 << (random_lim(CHARS));

				uint8_t tmp = solutions[i].genes[id2] & mask;
				solutions[i].genes[id2] =
					(solutions[i].genes[id2] & ~mask)
					| (solutions[i].genes[id1]);
				solutions[i].genes[id1] =
					(solutions[i].genes[id1] & ~mask)
					| tmp;
			}

			mut = random_lim(100);
			if (mut < 50) {
				size_t id1 = random_lim(STAGES);
				size_t id2 = random_lim(STAGES);
				uint8_t mask1 = 1 << (random_lim(CHARS));
				uint8_t mask2 = 1 << (random_lim(CHARS));

				uint8_t tmp1 = solutions[i].genes[id2] & mask1;
				uint8_t tmp2 = solutions[i].genes[id2] & mask2;

				solutions[i].genes[id2] =
					(solutions[i].genes[id2] & ~mask1)
					| (solutions[i].genes[id1] & mask1);
				solutions[i].genes[id2] =
					(solutions[i].genes[id2] & ~mask2)
					| (solutions[i].genes[id1] & mask2);

				solutions[i].genes[id1] =
					(solutions[i].genes[id1] & ~mask1)
					| tmp1;
				solutions[i].genes[id1] =
					(solutions[i].genes[id1] & ~mask2)
					| tmp2;
			}

			if (validate_genes(solutions[i].genes)) {
				solutions[i].time = time_stages(solutions[i].genes, agilities);
				solutions[i].fitness = fitness(solutions[i]);
				++i;
			}
		}

		solutions_num += INDV_STEP;
		if (solutions_num > INDV_CAP)
			solutions_num = cut_solutions(solutions, INDV_CAP, solutions_num);
	}

	time = solutions[0].time;

	free(solutions);

	return time;
}

/* TODO: Implement random removal here */
static size_t cut_solutions(Solution *const solutions, const size_t solutions_cap, const size_t solutions_num) {
	qsort(solutions, solutions_num, sizeof(Solution), &solution_sort);

	return solutions_cap;
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


static inline uint64_t next(const uint64_t v) {
	#define t (v | (v - 1))
	return (t + 1) | (((~t & - ~t) - 1) >> (__builtin_ctz(v) + 1));
	#undef t
}

static size_t generate_possible_genes(struct array **possible_genes) {
	uint64_t gene;

	*possible_genes = array_new(sizeof(uint64_t));

	for (gene = 0b11111111; !(gene & (1 << (STAGES))); gene = next(gene))
		array_push(*possible_genes, &gene);

	return array_get_size(*possible_genes);
}

static int generate_genes(Solution *solutions, const size_t size, const double agilities[CHARS]) {
	struct array *possible_genes;
	size_t i, j, k, possible_genes_num;

	possible_genes_num = generate_possible_genes(&possible_genes);

	for (i = 0; i < size;) {
		uint64_t char_genes[CHARS];
		for (j = 0; j < CHARS; ++j) {
			memcpy(&char_genes[j], array_get(possible_genes, random_lim(possible_genes_num)), sizeof(uint64_t));
		}

		memset(solutions[i].genes, 0, sizeof(uint8_t) * STAGES);

		for (j = 0; j < STAGES; ++j) {
			for (k = 0; k < CHARS; ++k) {
				if (char_genes[k] & (1 << j))
					solutions[i].genes[j] |= (1 << k);
			}
		}

		if (validate_genes(solutions[i].genes)) {
			solutions[i].time = time_stages(solutions[i].genes, agilities);
			solutions[i].fitness = fitness(solutions[i]);
			++i;
		}
	}

	array_free(possible_genes);

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
		printf("\t%.06f\n", solutions[i].time);
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

static double total_fitness(const Solution *const solutions, size_t solutions_num) {
	size_t i;
	double sum = 0;

	for (i = 0; i < solutions_num; ++i)
		sum += solutions[i].fitness;

	return sum;
}

static size_t roulette(const Solution *const solutions, const size_t solutions_num, const double total_fitness) {
	size_t i;
	double r = random() / (double)RAND_MAX;

	for (i = 0; i < solutions_num; ++i) {
		double pi = solutions[i].fitness / total_fitness;
		if (r < pi) {
			return i;
		}
		r -= pi;
	}

	return -1;
}

static double fitness(const Solution solution) {
	static const double scale = 10000;
	return solution.time > 0.0 ?  1.0 / solution.time * scale : DBL_MAX;
}

int solution_sort(const void *ptr1, const void *ptr2) {
	if (((Solution *)ptr1)->fitness < ((Solution *)ptr2)->fitness)
		return 1;
	else if (((Solution *)ptr1)->fitness > ((Solution *)ptr2)->fitness)
		return -1;
	else
		return 0;
}

