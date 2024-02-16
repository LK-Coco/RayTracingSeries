#pragma once

#include "hittable_list.h"

namespace cray {

class BVHNode : public Hittable {
public:
    BVHNode(const HittableList& list)
        : BVHNode(list.objects, 0, list.objects.size()) {}
    BVHNode(const std::vector<std::shared_ptr<Hittable>>& objs,
            size_t index_start, size_t index_end);

    bool hit(const Ray& ray, const Interval& interval,
             HitRecord& rec) const override;

    AABB bounding_box() const override { return aabb; }

    std::shared_ptr<Hittable> left;
    std::shared_ptr<Hittable> right;
    AABB aabb;
};

}  // namespace cray