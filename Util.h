#ifndef UTIL_H
#define UTIL_H

#include <cstdlib>

#include "Constants.h"

#ifdef DEBUG
#define P(x) printf x
#else
#define P(x)
#endif

inline float random_uniform_float(const float l_, const float h_) {
	return rand() / float(RAND_MAX) * (h_-l_) + l_;
}

inline void getResolutionArray(float ratio_, uint16_t*** ptr_) {
	float d16x9 = abs(ratio_ - SIXTEEN_BY_NINE);
	float d16x10 = abs(ratio_ - SIXTEEN_BY_TEN);
	float d4x3 = abs(ratio_ - FOUR_BY_THREE);

	*ptr_ = (uint16_t **)fourbythree;
	//if (d16x9 < d16x10 && d16x9 < d4x3) {
	//	return reinterpret_cast<uint16_t **>(const_cast<uint16_t(*)[2]>(sixteenbynine));
	//}
	//else if (d16x10 < d16x9 && d16x10 < d4x3) {
	//	return reinterpret_cast<uint16_t **>(const_cast<uint16_t(*)[2]>(sixteenbyten));
	//}
	//else {
	//	return reinterpret_cast<uint16_t **>(const_cast<uint16_t(*)[2]>(fourbythree));
	//}
}

#endif
