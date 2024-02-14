#include "hittable_list.h"

namespace cray {

bool HittableList::hit(const Ray& ray, const Interval& interval,
                       HitRecord& rec) const {
    HitRecord temp_rec;
    bool hit_anything = false;
    auto closest_so_far = interval.max;

    for (const auto& object : objects) {
        if (object->hit(ray, Interval(interval.min, closest_so_far),
                        temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }

    return hit_anything;
}

}  // namespace cray