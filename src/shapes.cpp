#include "shapes.h"

namespace cray {

// 从笛卡尔坐标系->极坐标
void get_sphere_uv(const Vec3& p, double& u, double& v) {
    auto theta = acos(-p.y);
    auto phi = atan2(-p.z, p.x) + PI;

    u = phi / (2 * PI);
    v = theta / PI;
}

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
    get_sphere_uv(outway_normal, rec.u, rec.v);

    return true;
}

bool Quad::hit(const Ray& ray, const Interval& interval, HitRecord& rec) const {
    auto denom = dot(normal, ray.dir);  // 分母
    if (fabs(denom) < 1e-8) return false;

    auto t = (D - dot(normal, ray.origin)) / denom;
    if (!interval.contains(t)) return false;

    // 判断交点是否在Quad内部
    auto intersection = ray.at(t);
    Vec3 p = intersection - Q;
    auto alpha = dot(W, cross(p, v));
    auto beta = dot(W, cross(u, p));
    if (alpha < 0 || alpha > 1 || beta < 0 || beta > 1) return false;
    rec.u = alpha;
    rec.v = beta;

    rec.t = t;
    rec.p = intersection;
    rec.mat = mat;
    rec.set_front_normal(ray, normal);

    return true;
}

}  // namespace cray