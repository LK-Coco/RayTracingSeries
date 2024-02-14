#pragma once

#include "common.h"

namespace cray {

struct Interval {
    Interval() : min(+Infinity), max(-Infinity) {}
    Interval(double min_v, double max_v) : min(min_v), max(max_v) {}
    Interval(const Interval& a, const Interval& b)
        : min(fmin(a.min, b.min)),
          max(fmax(a.max, b.max)) {}

    bool contains(double x) const { return min <= x && x <= max; }

    bool surrounds(double x) const { return min < x && x < max; }

    double clamp(double x) const {
        if (x < min) return min;
        if (x > max) return max;
        return x;
    }

    double size() const { return max - min; }

    Interval expand(double delta) const {
        auto padding = delta / 2.0;
        return Interval(min - padding, max + padding);
    }

    static const Interval empty, universe;

    double min, max;
};

const static Interval empty(+Infinity, -Infinity);
const static Interval universe(-Infinity, +Infinity);

}  // namespace cray