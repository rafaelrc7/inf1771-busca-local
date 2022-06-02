#include <float.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "genetic.h"

#include "array.h"
#include "random_utils.h"
#include "settings.h"

#define PRINT_GENS

struct individual {
	uint8_t *genes;
	double time;
	double fitness;
};
typedef struct individual Individual;

static int validate_genes(const uint8_t *const genes,
						  const Settings *const s);
static int generate_genes(Individual *population,
						  const size_t size,
						  const Settings *const s);
static void print_population(const Individual *const population,
							 const size_t size,
							 const size_t cap,
							 const Settings *const s);
static double time_stages(const uint8_t *const genes,
						  const Settings *const s);
static size_t roulette(const Individual *const population,
					   const size_t population_size,
					   const double total_fitness);
static double total_fitness(const Individual *const population,
							size_t population_size);
static double fitness(const Individual solution);
static size_t remove_duplicates(Individual *const population,
								size_t population_size);
int solution_sort(const void *ptr1, const void *ptr2);

double gen_solve(const Settings *const s)
{
	Individual *population;
	size_t generation, population_size, c;
	double time;

	population = (Individual *)calloc(s->population_cap + s->population_step, sizeof(Individual));
	for (c = 0; c < s->population_cap + s->population_step; ++c)
		population[c].genes = (uint8_t *)calloc(s->stage_num, sizeof(uint8_t));

	generate_genes(population, s->population_step, s);
	population_size = s->population_step;

	qsort(population, population_size, sizeof(Individual), &solution_sort);

	for (generation = 0; generation < s->generation_num; ++generation) {
		size_t i, j;
		double total_fitness_val = total_fitness(population, population_size);

		#ifdef PRINT_GENS
		printf("\n----- GENERATION %lu (%lu/%lu) -----\n", generation, population_size, s->population_cap);
		print_population(population, population_size, 10, s);
		#endif

		for (i = population_size; i < population_size + s->population_step;) {
			size_t parent1 = roulette(population, population_size, total_fitness_val);
			size_t parent2 = roulette(population, population_size, total_fitness_val);
			uint8_t mut;

			for (j = 0; j < s->stage_num; ++j) {
				uint8_t select = random_prob(2);
				population[i].genes[j] = select ?
					population[parent1].genes[j] : population[parent2].genes[j];
			}

			mut = random_lim(100);
			if (mut < s->mut_base) {
				size_t id1 = random_lim(s->stage_num);
				size_t id2 = random_lim(s->stage_num);
				uint8_t mask1 = 1 << (random_lim(s->char_num));
				uint8_t mask2 = 1 << (random_lim(s->char_num));

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

			mut = random_lim(100);
			if (mut < s->mut_base) {
				size_t id1 = random_lim(s->stage_num);
				size_t id2 = random_lim(s->stage_num);

				uint8_t tmp = population[i].genes[id1];
				population[i].genes[id1] = population[i].genes[id2];
				population[i].genes[id2] = tmp;

			}

			if (validate_genes(population[i].genes, s)) {
				population[i].time = time_stages(population[i].genes, s);
				population[i].fitness = fitness(population[i]);
				++i;
			}
		}

		population_size += s->population_step;
		qsort(population, population_size, sizeof(Individual), &solution_sort);
		population_size = remove_duplicates(population, population_size);
		if (population_size > s->population_cap) {
			population_size = s->population_cap;
		}
	}

	time = population[0].time;

	for (c = 0; c < s->population_cap + s->population_step; ++c)
		free(population[c].genes);

	free(population);

	return time;
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
				should_be_removed[j] = 1;
		}
	}

	for (i = population_size-1; i > 0; --i) {
		if (should_be_removed[i]) {
			if (i != population_size-1) {
				uint8_t *tmp = population[i].genes;
				memcpy(&population[i], &population[i+1], sizeof(Individual) * (population_size - i - 1));
				population[population_size - 1].genes = tmp;
			}
			--population_size;
		}
	}

	free(should_be_removed);

	return population_size;
}

static int validate_genes(const uint8_t *const genes, const Settings *const s) {
	size_t i, j, char_total = 0;
	uint8_t *chars = (uint8_t *)calloc(s->char_num, sizeof(uint8_t));
	memset(chars, 0, s->char_num * sizeof(uint8_t));

	for (i = 0; i < s->stage_num; ++i) {
		uint8_t mask = 1;
		if (!genes[i])
			goto false;

		for (j = 0; j < s->char_num; ++j, mask <<= 1) {
			if (mask & genes[i]) {
				if (++char_total > (s->char_num * 8 - 1))
					goto false;
				if (++chars[j] > 8)
					goto false;
			}
		}
	}

	free(chars);
	return 1;

	false:
	free(chars);
	return 0;
}


static inline uint64_t next(const uint64_t v) {
	#define t (v | (v - 1))
	return (t + 1) | (((~t & - ~t) - 1) >> (__builtin_ctz(v) + 1));
	#undef t
}

static size_t generate_possible_genes(struct array **possible_genes, const Settings *s) {
	uint64_t gene;

	*possible_genes = array_new(sizeof(uint64_t));

	for (gene = (1 << s->char_lives) - 1; !(gene & (1 << (s->stage_num))); gene = next(gene))
		array_push(*possible_genes, &gene);

	return array_get_size(*possible_genes);
}

static int generate_genes(Individual *population, const size_t size, const Settings *const s) {
	struct array *possible_genes;
	size_t i, j, k, possible_genes_num;

	uint64_t *char_genes = (uint64_t *)calloc(s->char_num, sizeof(uint64_t));
	if (char_genes == NULL)
		exit(EXIT_FAILURE);

	possible_genes_num = generate_possible_genes(&possible_genes, s);

	for (i = 0; i < size;) {
		for (j = 0; j < s->char_num; ++j) {
			memcpy(&char_genes[j], array_get(possible_genes, random_lim(possible_genes_num)), sizeof(uint64_t));
		}

		memset(population[i].genes, 0, sizeof(uint8_t) * s->stage_num);

		for (j = 0; j < s->stage_num; ++j) {
			for (k = 0; k < s->char_num; ++k) {
				if (char_genes[k] & (1 << j))
					population[i].genes[j] |= (1 << k);
			}
		}

		for (j = 0; j < s->stage_num; ++j) {
			if (population[i].genes[j] != (1 << (s->char_num-1)) && population[i].genes[j] &  (1 << (s->char_num-1))) {
				population[i].genes[j] ^= (1 << (s->char_num-1));
			}
		}

		if (validate_genes(population[i].genes, s)) {
			population[i].time = time_stages(population[i].genes, s);
			population[i].fitness = fitness(population[i]);
			++i;
		}
	}

	array_free(possible_genes);
	free(char_genes);

	return 1;
}

static void print_population(const Individual *const population, const size_t size, const size_t cap, const Settings *const s) {
	size_t i, j, k;
	char buff[s->char_num + 1];

	for (i = 0; i < cap && i < size; ++i) {
		for (j = 0; j < s->stage_num; ++j) {
			uint8_t mask = 1 << (s->char_num - 1);
			for (k = 0; k < s->char_num; ++k, mask >>= 1) {
				buff[k] = population[i].genes[j] & mask ? '1' : '0';
			}
			buff[k] = 0;
			printf("%s ", buff);
		}
		printf("\t%.010f\n", population[i].time);
	}
}

static double time_stage(const uint8_t gene, const uint8_t stage, const Settings *const s) {
	size_t i;
	uint8_t mask;
	double diff = s->difficulties[stage];
	double ag_sum = 0;

	for (i = 0, mask = 1; i < s->char_num; ++i, mask <<= 1) {
		if (gene & mask)
			ag_sum += s->agilities[i];
	}

	return diff / ag_sum;
}

static double time_stages(const uint8_t *const genes, const Settings *const s) {
	size_t i;
	double time = 0;

	for (i = 0; i < s->stage_num; ++i) {
		time += time_stage(genes[i], i, s);
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

