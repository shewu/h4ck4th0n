#ifndef UTIL_H
#define UTIL_H

#include <cstdlib>

#include "Constants.h"

#ifdef DEBUG
#define P(x) printf x
#else
#define P(x)
#endif

#define TWO_PI (2.0 * M_PI);

// Returns a mod m as an integer in [0, m)
inline int modHandleNegative(int a, int m) {
	a %= m;
	if (a < 0) {
		return a + m;
	} else {
		return a;
	}
}

inline float random_uniform_float(const float l_, const float h_) {
	return rand() / float(RAND_MAX) * (h_-l_) + l_;
}

template<typename T_>
inline int argMin(std::initializer_list<T_> args_) {
	T_ minVal = *args_.begin();
	int minInd = 0;
	int ctr = 0;
	for (auto obj : args_) {
		if (obj < minVal) {
			minInd = ctr;
		}
		++ctr;
	}
	return minInd;
}

#endif
