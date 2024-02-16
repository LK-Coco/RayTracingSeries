#pragma once

#include "hit_record.h"
#include "interval.h"
#include "ray.h"
#include "aabb.h"

namespace cray {

class Hittable {
public:
    virtual ~Hittable() = default;

    virtual bool hit(const Ray& ray, const Interval& interval,
                     HitRecord& rec) const = 0;

    virtual AABB bounding_box() const = 0;
};

}  // namespace cray