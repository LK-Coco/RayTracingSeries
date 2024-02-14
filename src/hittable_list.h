#pragma once

#include <memory>
#include <vector>
#include "hittable.h"

namespace cray {

struct HittableList : public Hittable {
    HittableList() {}
    HittableList(std::shared_ptr<Hittable> obj) { add(obj); }

    void add(std::shared_ptr<Hittable> obj) {
        objects.push_back(obj);
        aabb = AABB(aabb, obj->bounding_box());
    }

    void clear() { objects.clear(); }

    bool hit(const Ray& ray, const Interval& interval,
             HitRecord& rec) const override;

    AABB bounding_box() const override { return aabb; }

    std::vector<std::shared_ptr<Hittable>> objects;

    AABB aabb;
};

}  // namespace cray