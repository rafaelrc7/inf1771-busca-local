/* Credits to: http://c-faq.com/lib/randrange.html */

#include <stdlib.h>

long random_lim(const long n) {
	return (long)((double)random() / ((double)RAND_MAX + 1) * n);
}

double drandom_lim(const double n) {
	return (double)random() / (RAND_MAX / n + 1);
}

long random_range(const long m, const long n) {
	return (long)(m + (double)random() / ((double)RAND_MAX / (n - m + 1) + 1));
}

double drandom_range(const double m, const double n) {
	return m + (double)random() / (RAND_MAX / (n - m + 1) + 1);
}

int random_prob(const double n) {
	return random() < ((RAND_MAX+1u) / n);
}

