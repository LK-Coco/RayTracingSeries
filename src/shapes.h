#pragma once

#include "hittable.h"

namespace cray {

class Sphere : public Hittable {
public:
    // 静态球体
    Sphere(Point3 _center, double _radius, std::shared_ptr<Material> _mat)
        : center(_center),
          radius(_radius),
          mat(_mat),
          is_moving(false) {
        auto space = Vec3(radius, radius, radius);
        aabb = AABB(center - space, center + space);
    }

    // 动态球体
    Sphere(Point3 _center, Point3 move_target, double _radius,
           std::shared_ptr<Material> _mat)
        : center(_center),
          radius(_radius),
          mat(_mat),
          is_moving(true) {
        move_vec = move_target - _center;

        auto space = Vec3(radius, radius, radius);
        AABB box0(center - space, center + space);
        AABB box1(move_target - space, move_target + space);
        aabb = AABB(box0, box1);
    }

    Vec3 get_cur_center(double time) const { return center + time * move_vec; }

    bool hit(const Ray& ray, const Interval& interval,
             HitRecord& rec) const override;

    AABB bounding_box() const override { return aabb; }

    Point3 center;
    double radius;
    std::shared_ptr<Material> mat;

    bool is_moving;
    Vec3 move_vec;

    AABB aabb;
};

class Quad : public Hittable {
public:
    Quad(const Point3& _Q, const Vec3& _u, const Vec3& _v,
         std::shared_ptr<Material> m)
        : Q(_Q),
          u(_u),
          v(_v),
          mat(m) {
        auto n = cross(u, v);
        W = n / dot(n, n);
        normal = unit_vector(n);
        D = dot(normal, Q);
        set_bounding_box();
    }

    virtual void set_bounding_box() { bbox = AABB(Q, Q + u + v).pad(); }

    AABB bounding_box() const override { return bbox; }

    bool hit(const Ray& ray, const Interval& interval,
             HitRecord& rec) const override;

private:
    Point3 Q;
    Vec3 u, v;  // 两条边的向量
    std::shared_ptr<Material> mat;
    AABB bbox;

    Vec3 normal;
    double D;
    Vec3 W;
};

}  // namespace cray