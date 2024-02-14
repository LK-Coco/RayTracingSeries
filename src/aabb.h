#pragma once

#include "cgmath.h"
#include "interval.h"
#include "ray.h"

namespace cray {

struct AABB {
    AABB() {}
    AABB(const Interval& ix, const Interval& iy, const Interval& iz)
        : x(ix),
          y(iy),
          z(iz) {}
    AABB(const Point3& a, const Point3& b) {
        x = Interval(fmin(a.x, b.x), fmax(a.x, b.x));
        y = Interval(fmin(a.y, b.y), fmax(a.y, b.y));
        z = Interval(fmin(a.z, b.z), fmax(a.z, b.z));
    }
    AABB(const AABB& box0, const AABB& box1) {
        x = Interval(box0.x, box1.x);
        y = Interval(box0.y, box1.y);
        z = Interval(box0.z, box1.z);
    }

    const Interval& axis(int n) const {
        if (n == 1) return y;
        if (n == 2) return z;
        return x;
    }

    bool hit(const Ray& ray, Interval interval) const {
        for (int n = 0; n < 3; ++n) {
            auto invD = 1 / ray.dir[n];
            auto orig = ray.origin[n];

            auto t0 = (axis(n).min - orig) * invD;
            auto t1 = (axis(n).max - orig) * invD;

            if (invD < 0) std::swap(t0, t1);

            if (t0 > interval.min) interval.min = t0;
            if (t1 < interval.max) interval.max = t1;

            if (interval.max <= interval.min) return false;
        }

        return true;
    }

    Interval x, y, z;
};

}  // namespace cray