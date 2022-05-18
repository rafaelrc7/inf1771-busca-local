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

#define START_POOL 500

uint64_t next(uint64_t v);
void random_generation(uint64_t genes[CHARS], struct array *possible_genes);
ssize_t starting_generations(uint64_t (*genes)[CHARS], size_t max, unsigned int seed);
double time_stages(const uint64_t genes[CHARS], const double agilities[CHARS]);

int main(void) {
	static const double agilities[] = {1.8, 1.6, 1.6, 1.6, 1.4, 0.9, 0.7};
	size_t valid_generations = 0;
	uint64_t (*genes)[CHARS];
	char buff[31] = {0};
	size_t i, j, k;

	genes = malloc(sizeof(genes[CHARS]) * START_POOL);

	valid_generations = starting_generations(genes, START_POOL, time(0));

	for (i = 0; i < valid_generations; ++i) {
		for (j = 0; j < CHARS; ++j) {
			uint64_t mask = 1 << (STAGES-1);
			for (k = 0; k < STAGES; ++k, mask >>= 1) {
				buff[k] = genes[i][j] & mask ? '1' : '0';
			}
			printf("%s ", buff);
		}
		printf("\t%.02f\n", time_stages(genes[i], agilities));
	}

	free(genes);

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

int validate_genes(uint64_t genes[CHARS]) {
	uint64_t mask = 0;
	size_t i;

	for (i = 0; i < CHARS; ++i)
		mask |= genes[i];

	return mask == (GENE_MASK);
}

ssize_t starting_generations(uint64_t (*genes)[CHARS], size_t max, unsigned int seed) {
	struct array *possible_genes = array_new(sizeof(uint64_t));
	uint64_t gene;
	size_t i, j;

	for (gene = START_GENE; !(gene & (1 << (STAGES))); gene = next(gene))
		array_push(possible_genes, &gene);

	if (genes == NULL)
		return -1;

	srand(seed);

	for (i = 0, j = 0; i < max; ++i) {
		random_generation(genes[j], possible_genes);
		if (validate_genes(genes[j])) {
			++j;
		}
	}

	array_free(possible_genes);

	return j;
}

void random_generation(uint64_t genes[CHARS], struct array *possible_genes) {
	size_t i;

	for (i = 0; i < CHARS; ++i) {
		memcpy(&genes[i], array_get(possible_genes, rand() % array_get_size(possible_genes)), sizeof(uint64_t));
	}
}

