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

#define MUT_BASE	95

struct individual {
	uint8_t genes[STAGES];
	double time;
	double fitness;
};
typedef struct individual Individual;

static int validate_genes(const uint8_t genes[STAGES]);
static int generate_genes(Individual *population, const size_t size, const double agilities[CHARS]);
static void print_population(const Individual *const population, const size_t size, const size_t cap);
static double time_stages(const uint8_t genes[STAGES], const double agilities[CHARS]);
static size_t roulette(const Individual *const population, const size_t population_size, const double total_fitness);
static double total_fitness(const Individual *const population, size_t population_size);
static double fitness(const Individual solution);
static size_t cut_population(Individual *const population, const size_t population_cap, const size_t population_size);
static size_t remove_duplicates(Individual *const population, size_t population_size);
int solution_sort(const void *ptr1, const void *ptr2);

double gen_solve(const double agilities[CHARS], const size_t generation_num,
				 const size_t pop_step, const size_t pop_cap)
{
	Individual *population;
	size_t generation, population_size;
	double time;

	population = (Individual *)calloc(pop_cap + pop_step, sizeof(Individual));

	generate_genes(population, pop_step, agilities);
	population_size = pop_step;

	qsort(population, population_size, sizeof(Individual), &solution_sort);

	for (generation = 0; generation < generation_num; ++generation) {
		size_t i, j;
		double total_fitness_val = total_fitness(population, population_size);

		#ifdef PRINT_GENS
		printf("\n----- GENERATION %lu (%lu/%lu) -----\n", generation, population_size, pop_cap);
		print_population(population, population_size, 10);
		#endif

		for (i = population_size; i < population_size + pop_step;) {
			size_t parent1 = roulette(population, population_size, total_fitness_val);
			size_t parent2 = roulette(population, population_size, total_fitness_val);
			uint8_t mut;

			for (j = 0; j < STAGES; ++j) {
				uint8_t select = random_prob(2);
				population[i].genes[j] = select ?
					population[parent1].genes[j] : population[parent2].genes[j];
			}

			mut = random_lim(100);
			if (mut < MUT_BASE * generation/generation_num + 1) {
				size_t id1 = random_lim(STAGES);
				size_t id2 = random_lim(STAGES);
				uint8_t mask1 = 1 << (random_lim(CHARS));
				uint8_t mask2 = 1 << (random_lim(CHARS));

				uint8_t tmp1 = population[i].genes[id2] & mask1;
				uint8_t tmp2 = population[i].genes[id2] & mask2;

				population[i].genes[id2] =
					(population[i].genes[id2] & ~mask1)
					| (population[i].genes[id1] & mask1);
				population[i].genes[id2] =
					(population[i].genes[id2] & ~mask2)
					| (population[i].genes[id1] & mask2);

				population[i].genes[id1] =
					(population[i].genes[id1] & ~mask1)
					| tmp1;
				population[i].genes[id1] =
					(population[i].genes[id1] & ~mask2)
					| tmp2;
			}

			if (validate_genes(population[i].genes)) {
				population[i].time = time_stages(population[i].genes, agilities);
				population[i].fitness = fitness(population[i]);
				++i;
			}
		}

		population_size += pop_step;
		qsort(population, population_size, sizeof(Individual), &solution_sort);
		population_size = remove_duplicates(population, population_size);
		if (population_size > pop_cap)
			population_size = cut_population(population, pop_cap, population_size);
	}

	time = population[0].time;

	free(population);

	return time;
}

/* TODO: Implement random removal here */
static size_t cut_population(Individual *const population, const size_t population_cap, const size_t population_size) {
	return population_cap;
}

static size_t remove_duplicates(Individual *const population, size_t population_size) {
	size_t i;
	uint8_t *should_be_removed = (uint8_t *)calloc(population_size, sizeof(uint8_t));

	for (i = 1; i < population_size; ++i) {
		size_t j;
		double fitness = population[i-1].fitness;

		if (should_be_removed[i])
			continue;

		for (j = i; population[j].fitness == fitness && j < population_size; ++j) {
			if (memcmp(population[j].genes, population[i-1].genes, sizeof(uint8_t) * STAGES) == 0)
				should_be_removed[j] = 1;
		}
	}

	for (i = population_size-1; i > 0; --i) {
		if (should_be_removed[i]) {
			if (i != population_size-1)
				memcpy(&population[i], &population[i+1], sizeof(Individual) * (population_size - i - 1));
			--population_size;
		}
	}

	return population_size;
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

static int generate_genes(Individual *population, const size_t size, const double agilities[CHARS]) {
	struct array *possible_genes;
	size_t i, j, k, possible_genes_num;

	possible_genes_num = generate_possible_genes(&possible_genes);

	for (i = 0; i < size;) {
		uint64_t char_genes[CHARS];
		for (j = 0; j < CHARS; ++j) {
			memcpy(&char_genes[j], array_get(possible_genes, random_lim(possible_genes_num)), sizeof(uint64_t));
		}

		memset(population[i].genes, 0, sizeof(uint8_t) * STAGES);

		for (j = 0; j < STAGES; ++j) {
			for (k = 0; k < CHARS; ++k) {
				if (char_genes[k] & (1 << j))
					population[i].genes[j] |= (1 << k);
			}
		}

		if (validate_genes(population[i].genes)) {
			population[i].time = time_stages(population[i].genes, agilities);
			population[i].fitness = fitness(population[i]);
			++i;
		}
	}

	array_free(possible_genes);

	return 1;
}

static void print_population(const Individual *const population, const size_t size, const size_t cap) {
	size_t i, j, k;
	char buff[CHARS + 1];

	for (i = 0; i < cap && i < size; ++i) {
		for (j = 0; j < STAGES; ++j) {
			uint8_t mask = 1 << (CHARS - 1);
			for (k = 0; k < CHARS; ++k, mask >>= 1) {
				buff[k] = population[i].genes[j] & mask ? '1' : '0';
			}
			buff[k] = 0;
			printf("%s ", buff);
		}
		printf("\t%.06f\n", population[i].time);
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

static double total_fitness(const Individual *const population, size_t population_size) {
	size_t i;
	double sum = 0;

	for (i = 0; i < population_size; ++i)
		sum += population[i].fitness;

	return sum;
}

static size_t roulette(const Individual *const population, const size_t population_size, const double total_fitness) {
	size_t i;
	double r = random() / (double)RAND_MAX;

	for (i = 0; i < population_size; ++i) {
		double pi = population[i].fitness / total_fitness;
		if (r < pi) {
			return i;
		}
		r -= pi;
	}

	return -1;
}

static double fitness(const Individual solution) {
	static const double scale = 10000;
	return solution.time > 0.0 ?  1.0 / solution.time * scale : DBL_MAX;
}

int solution_sort(const void *ptr1, const void *ptr2) {
	if (((Individual *)ptr1)->fitness < ((Individual *)ptr2)->fitness)
		return 1;
	else if (((Individual *)ptr1)->fitness > ((Individual *)ptr2)->fitness)
		return -1;
	else
		return 0;
}

