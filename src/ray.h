#pragma once

#include "cgmath.h"

namespace cray {

struct Ray {
    Ray() {}
    Ray(const Point3& o, const Vec3& d) : origin(o), dir(d), tm(0) {}
    Ray(const Point3& o, const Vec3& d, double time)
        : origin(o),
          dir(d),
          tm(time) {}

    Point3 at(double t) const { return origin + dir * t; }

    Point3 origin;
    Vec3 dir;

    double tm;  // 动画帧的时间
};

}  // namespace cray