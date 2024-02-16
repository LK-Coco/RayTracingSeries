#include "bvh.h"

namespace cray {

bool box_compare(const std::shared_ptr<Hittable> a,
                 const std::shared_ptr<Hittable> b, int axis_index) {
    return a->bounding_box().axis(axis_index).min <
           b->bounding_box().axis(axis_index).min;
}

bool box_compare_x(const std::shared_ptr<Hittable> a,
                   const std::shared_ptr<Hittable> b) {
    return box_compare(a, b, 0);
}

bool box_compare_y(const std::shared_ptr<Hittable> a,
                   const std::shared_ptr<Hittable> b) {
    return box_compare(a, b, 1);
}

bool box_compare_z(const std::shared_ptr<Hittable> a,
                   const std::shared_ptr<Hittable> b) {
    return box_compare(a, b, 2);
}

BVHNode::BVHNode(const std::vector<std::shared_ptr<Hittable>>& objs,
                 size_t index_start, size_t index_end) {
    auto copy_objs = objs;

    int axis = random_int(0, 2);

    auto comparator = axis == 0   ? box_compare_x
                      : axis == 1 ? box_compare_y
                                  : box_compare_z;

    size_t objs_num = index_end - index_start;

    if (objs_num == 1) {
        left = right = copy_objs[index_start];
    } else if (objs_num == 2) {
        if (comparator(copy_objs[index_start], copy_objs[index_start + 1])) {
            left = copy_objs[index_start];
            right = copy_objs[index_start + 1];
        } else {
            left = copy_objs[index_start + 1];
            right = copy_objs[index_start];
        }
    } else {
        std::sort(copy_objs.begin() + index_start,
                  copy_objs.begin() + index_end, comparator);
        size_t mid = index_start + objs_num / 2;
        left = std::make_shared<BVHNode>(objs, index_start, mid);
        right = std::make_shared<BVHNode>(objs, mid, index_end);
    }

    aabb = AABB(left->bounding_box(), right->bounding_box());
}

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