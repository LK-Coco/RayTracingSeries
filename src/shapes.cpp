#include "shapes.h"

namespace cray {

bool Sphere::hit(const Ray& ray, const Interval& interval,
                 HitRecord& rec) const {
    // 光线方程o+t*d带入球方程p*p - r*r=0
    Point3 cur_center = is_moving ? get_cur_center(ray.tm) : center;
    Vec3 oc = ray.origin - cur_center;
    auto a = ray.dir.length_sq();
    auto half_b = dot(oc, ray.dir);
    auto c = oc.length_sq() - radius * radius;

    auto discriminant = half_b * half_b - a * c;
    if (discriminant < 0) return false;
    auto sqrtd = sqrt(discriminant);

    auto root = (-half_b - sqrtd) / a;
    if (!interval.surrounds(root)) {
        root = (-half_b + sqrtd) / a;
        if (!interval.surrounds(root)) return false;
    }

    rec.t = root;
    rec.p = ray.at(rec.t);
    rec.mat = mat;
    auto outway_normal = (rec.p - cur_center) / radius;
    rec.set_front_normal(ray, outway_normal);

    return true;
}

}  // namespace cray