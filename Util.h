#ifndef UTIL_H
#define UTIL_H

#include <cstdlib>

#ifdef DEBUG
#define P(x) printf x
#else
#define P(x)
#endif

inline float random_uniform_float(float l, float h) {
	return rand() / float(RAND_MAX) * (h-l) + l;
}

#endif
