// This file is a general-purpose header that includes a number of common
// standard library headers, our own project headers, and utility functions
// that are used throughout the ray tracer.

#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <cmath>
#include <cstdlib> // For rand(), RAND_MAX
#include <limits> // For std::numeric_limits
#include <memory> // For shared_ptr

// Usings
using std::make_shared;
using std::shared_ptr;
using std::sqrt;
using std::fabs;

// Constants
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions
inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

inline double random_double() {
    // Returns a random real in [0,1).
    return rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max-min)*random_double();
}

inline int random_int(int min, int max) {
    // Returns a random integer in [min,max].
    return int(random_double(min, max+1));
}

// Common Headers
#include "ray.h"
#include "vec3.h"
#include "interval.h"

#endif
