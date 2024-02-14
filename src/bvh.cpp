#include "bvh.h"

namespace cray {

bool BVHNode::hit(const Ray& ray, const Interval& interval,
                  HitRecord& rec) const {
    if (!aabb.hit(ray, interval)) return false;

    // 计算左右叶节点的命中情况
    bool left_hit = left->hit(ray, interval, rec);
    bool right_hit = right->hit(
        ray, Interval(interval.min, left_hit ? rec.t : interval.max), rec);

    return left_hit || right_hit;
}

}  // namespace cray