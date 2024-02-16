#pragma once

#include <memory>
#include "cgmath.h"
#include "ray.h"

namespace cray {

struct Material;

struct HitRecord {
    Point3 p;
    Vec3 normal;
    double t;

    double u;
    double v;

    std::shared_ptr<Material> mat;

    bool is_front_face;

    void set_front_normal(const Ray& ray, const Vec3& outway_normal) {
        is_front_face = dot(ray.dir, outway_normal) < 0;
        normal = is_front_face ? outway_normal : -outway_normal;
    }
};

}  // namespace cray