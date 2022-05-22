#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "array.h"

#define STAGES	30
#define CHARS	7
#define BITS	((STAGES)*(CHARS))
#define BYTES	((((BITS) + 8 - 1) - (((BITS) + 8 - 1) % 8)) / 8)

#define GENE_MASK 0b111111111111111111111111111111
#define NTH_GENE_MASK(N) ((GENE_MASK) << ((STAGES) * N))
#define START_GENE 0b11111111

#define STAGE_DIFF(N) (((N) - 1) * 10)

#define START_POOL 1000000

#define GENERATIONS 10000
#define INDV_KEEP 	0
#define VALID_SONS	10000

struct solution {
	uint64_t genes[CHARS];
	double time;
};
typedef struct solution Solution;

uint64_t next(uint64_t v);
void random_generation(uint64_t genes[CHARS], struct array *possible_genes);
ssize_t starting_generations(Solution *solutions, size_t max, const double agilities[CHARS]);
double time_stages(const uint64_t genes[CHARS], const double agilities[CHARS]);
int validate_genes(uint64_t genes[CHARS]);
int compar(const void *, const void*);
void print_solutions(const Solution *solutions, const size_t cap, const size_t size);
static uint64_t swapbits(uint64_t n, uint8_t b1, uint8_t b2);
size_t weight_idx(size_t max);

int main(void) {
	struct array *possible_genes = array_new(sizeof(uint64_t));
	static const double agilities[] = {1.8, 1.6, 1.6, 1.6, 1.4, 0.9, 0.7};
	Solution *solutions;
	uint64_t gene;
	size_t individual_count = 0;
	size_t i;

	srandom(time(NULL));

	for (gene = START_GENE; !(gene & (1 << (STAGES))); gene = next(gene))
		array_push(possible_genes, &gene);

	solutions = malloc(sizeof(Solution) * START_POOL);

	individual_count = starting_generations(solutions, START_POOL, agilities);

	for (i = 0; i < GENERATIONS; ++i) {
		size_t indv = 0;
		size_t j;
		Solution *new_solutions;

		qsort(solutions, individual_count, sizeof(Solution), &compar);

		printf("\n----- GENERATION %lu -----\n", i);
		print_solutions(solutions, 5, individual_count);

		new_solutions = malloc(sizeof(Solution) * (VALID_SONS + INDV_KEEP));

		for (j = 0; j < INDV_KEEP; ++j) {
			memcpy(&new_solutions[indv + j], &solutions[j], sizeof(Solution));
		}
		indv += j;

		for (j = 0; j < VALID_SONS;) {
			size_t k;
			size_t selected1 = weight_idx(individual_count);
			size_t selected2 = weight_idx(individual_count);

			for (k = 0; k < CHARS; ++k) {
				uint8_t select = random() % 2;
				uint8_t mut = random() % 101;

				new_solutions[indv + j].genes[k] = select ? solutions[selected1].genes[k] : solutions[selected2].genes[k];

				if (mut < 5) {
					new_solutions[indv + j].genes[k] = swapbits(new_solutions[indv + j].genes[k], random() % STAGES, random() % STAGES);
				} else if (mut < 20) {
					new_solutions[indv +j].genes[k] = (new_solutions[indv + j].genes[k] << 15 & 0b111111111111111000000000000000) | (new_solutions[indv + j].genes[k] >> 15 & 0b111111111111111);
				}
			}

			if (validate_genes(new_solutions[indv + j].genes)) {
				new_solutions[indv + j].time = time_stages(new_solutions[indv + j].genes, agilities);
				++j;
			}
		}
		indv += j;
		individual_count = indv;

		free(solutions);
		solutions = new_solutions;
	}

	free(solutions);
	array_free(possible_genes);

	return 0;
}

uint64_t next(uint64_t v) {
	uint64_t t = v | (v - 1);
	return (t + 1) | (((~t & -~t) - 1) >> (__builtin_ctz(v) + 1));
}

double time_stage(const uint64_t genes[CHARS], const double agilities[CHARS], const int stage) {
	size_t i;
	double time = STAGE_DIFF(stage);
	double agility_sum = 0;

	for (i = 0; i < CHARS; ++i) {
		if (genes[i] & (1 << stage))
			agility_sum += agilities[i];
	}

	return time / agility_sum;
}

double time_stages(const uint64_t genes[CHARS], const double agilities[CHARS]) {
	double time = 0;
	size_t i;

	for (i = 0; i < STAGES; ++i)
		time += time_stage(genes, agilities, i);

	return time;
}

int validate_gene(uint64_t gene) {
	uint64_t mask = 1;
	size_t i;
	int count = 0;

	for (i = 0; i < STAGES; ++i) {
		if (gene & mask)
			++count;
		mask <<= 1;
	}

	return count;
}

int validate_genes(uint64_t genes[CHARS]) {
	uint64_t mask = 0;
	size_t i;

	for (i = 0; i < CHARS; ++i) {
		mask |= genes[i];
		if (validate_gene(genes[i]) != 8)
			return 0;
	}

	return mask == (GENE_MASK);
}

ssize_t starting_generations(Solution *solutions, size_t max, const double agilities[CHARS]) {
	struct array *possible_genes = array_new(sizeof(uint64_t));
	uint64_t gene;
	size_t i, j;

	for (gene = START_GENE; !(gene & (1 << (STAGES))); gene = next(gene))
		array_push(possible_genes, &gene);

	for (i = 0, j = 0; i < max; ++i) {
		random_generation(solutions[j].genes, possible_genes);

		if (validate_genes(solutions[j].genes)) {
			solutions[j].time = time_stages(solutions[j].genes, agilities);
			++j;
		}
	}

	array_free(possible_genes);

	return j;
}

void random_generation(uint64_t genes[CHARS], struct array *possible_genes) {
	size_t i;

	for (i = 0; i < CHARS; ++i) {
		memcpy(&genes[i], array_get(possible_genes, random() % array_get_size(possible_genes)), sizeof(uint64_t));
	}
}

int compar(const void *ptr1, const void*ptr2) {
	Solution *s1 = (Solution *)ptr1;
	Solution *s2 = (Solution *)ptr2;
	double diff = s1->time - s2->time;

	if (diff < 0.0L) {
		return -1;
	} else if (diff > 0.0F) {
		return 1;
	} else {
		return 0;
	}
}

void print_solutions(const Solution *solutions, const size_t cap, const size_t size) {
	size_t i, j, k;
	char buff[31] = {0};

	for (i = 0; i < cap && i < size; ++i) {
		for (j = 0; j < CHARS; ++j) {
			uint64_t mask = 1 << (STAGES-1);
			for (k = 0; k < STAGES; ++k, mask >>= 1) {
				buff[k] = solutions[i].genes[j] & mask ? '1' : '0';
			}
			printf("%s ", buff);
		}
		printf("\t%.02f\n", solutions[i].time);
	}
}

static uint64_t swapbits(uint64_t n, uint8_t b1, uint8_t b2) {
	uint64_t fstBit = (n >> b1) & 1;
	uint64_t sndBit = (n >> b2) & 1;

	uint64_t xorBit = fstBit ^ sndBit;

	xorBit = (xorBit << b1) | (xorBit << sndBit);

	return n ^ xorBit;
}

size_t weight_idx(size_t max) {
	uint8_t slice = random() % 100;

	if (slice < 30) {
		return random() % (size_t)(0.3 * max);
	} else if (slice < 70) {
		return (0.3 * max) + (random() % (size_t)(0.2 * max));
	} else if (slice < 90) {
		return (0.5 * max) + (random() % (size_t)(0.2 * max));
	} else {
		return (0.7 * max) + (random() % (size_t)(0.3 * max));
	}
}

