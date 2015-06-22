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

inline float random_uniform_float(const float l_, const float h_) {
    return rand() / static_cast<float>(RAND_MAX) * (h_ - l_) + l_;
}

template <typename T_>
inline int argMin(std::initializer_list<T_> args_) {
    T_ minVal = *args_.begin();
    int minInd = 0;
    int ctr = 0;
    for (auto obj : args_) {
        if (obj < minVal) {
            minInd = ctr;
            minVal = obj;
        }
        ++ctr;
    }
    return minInd;
}

#endif
