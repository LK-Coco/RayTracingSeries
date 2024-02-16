#pragma once

#include <limits>
#include <random>

// Constants

const double Infinity = std::numeric_limits<double>::infinity();
const double PI = 3.1415926535897932385;

// Utility Functions

inline double degrees_to_radians(double degrees) {
    return degrees * PI / 180.0;
}

// retrun [0,1)
inline double random_double() {
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}

// retrun [min,max)
inline double random_double(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max - min) * random_double();
}

// 返回[min,max]
inline int random_int(int min, int max) {
    return static_cast<int>(random_double(min, max + 1));
}
