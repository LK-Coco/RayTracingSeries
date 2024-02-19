#pragma once

#include "hit_record.h"
#include "interval.h"
#include "ray.h"
#include "aabb.h"
#include "material.h"

namespace cray {

class Hittable {
public:
    virtual ~Hittable() = default;

    virtual bool hit(const Ray& ray, const Interval& interval,
                     HitRecord& rec) const = 0;

    virtual AABB bounding_box() const = 0;
};

class Translate : public Hittable {
public:
    Translate(std::shared_ptr<Hittable> obj, const Vec3& offset)
        : obj_(obj),
          offset_(offset) {
        aabb_ = obj_->bounding_box() + offset_;
    }

    bool hit(const Ray& ray, const Interval& interval,
             HitRecord& rec) const override {
        Ray offset_ray = Ray(ray.origin - offset_, ray.dir, ray.tm);

        if (!obj_->hit(offset_ray, interval, rec)) return false;

        rec.p += offset_;
        return true;
    }

    AABB bounding_box() const override { return aabb_; }

private:
    std::shared_ptr<Hittable> obj_;
    Vec3 offset_;
    AABB aabb_;
};

class RotateY : public Hittable {
public:
    RotateY(std::shared_ptr<Hittable> p, double angle) : obj_(p) {
        auto radians = degrees_to_radians(angle);
        sin_theta_ = sin(radians);
        cos_theta_ = cos(radians);
        aabb_ = obj_->bounding_box();

        Point3 min(Infinity, Infinity, Infinity);
        Point3 max(-Infinity, -Infinity, -Infinity);

        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    auto x = i * aabb_.x.max + (1 - i) * aabb_.x.min;
                    auto y = j * aabb_.y.max + (1 - j) * aabb_.y.min;
                    auto z = k * aabb_.z.max + (1 - k) * aabb_.z.min;

                    auto newx = cos_theta_ * x + sin_theta_ * z;
                    auto newz = -sin_theta_ * x + cos_theta_ * z;

                    Vec3 tester(newx, y, newz);

                    for (int c = 0; c < 3; c++) {
                        min[c] = fmin(min[c], tester[c]);
                        max[c] = fmax(max[c], tester[c]);
                    }
                }
            }
        }

        aabb_ = AABB(min, max);
    }

    bool hit(const Ray& ray, const Interval& interval,
             HitRecord& rec) const override {
        auto origin = ray.origin;
        auto direction = ray.dir;

        origin[0] = cos_theta_ * ray.origin[0] - sin_theta_ * ray.origin[2];
        origin[2] = sin_theta_ * ray.origin[0] + cos_theta_ * ray.origin[2];

        direction[0] = cos_theta_ * ray.dir[0] - sin_theta_ * ray.dir[2];
        direction[2] = sin_theta_ * ray.dir[0] + cos_theta_ * ray.dir[2];

        Ray rotated_r(origin, direction, ray.tm);
        if (!obj_->hit(rotated_r, interval, rec)) return false;

        auto p = rec.p;
        p[0] = cos_theta_ * rec.p[0] + sin_theta_ * rec.p[2];
        p[2] = -sin_theta_ * rec.p[0] + cos_theta_ * rec.p[2];

        auto normal = rec.normal;
        normal[0] = cos_theta_ * rec.normal[0] + sin_theta_ * rec.normal[2];
        normal[2] = -sin_theta_ * rec.normal[0] + cos_theta_ * rec.normal[2];

        rec.p = p;
        rec.normal = normal;

        return true;
    }

    AABB bounding_box() const override { return aabb_; }

private:
    std::shared_ptr<Hittable> obj_;
    double sin_theta_;
    double cos_theta_;
    AABB aabb_;
};

class ConstantMedium : public Hittable {
public:
    ConstantMedium(std::shared_ptr<Hittable> obj, double d,
                   std::shared_ptr<Texture> a)
        : boundary_(obj),
          neg_inv_density_(-1 / d),
          mat_(std::make_shared<Isotropic>(a)) {}
    ConstantMedium(std::shared_ptr<Hittable> obj, double d, Color c)
        : boundary_(obj),
          neg_inv_density_(-1 / d),
          mat_(std::make_shared<Isotropic>(c)) {}

    bool hit(const Ray& ray, const Interval& interval,
             HitRecord& rec) const override {
        HitRecord rec1, rec2;

        // 判断光线是否穿过物体
        if (!boundary_->hit(ray, Interval::universe, rec1)) return false;

        if (!boundary_->hit(ray, Interval(rec1.t + 0.0001, Infinity), rec2))
            return false;

        if (rec1.t < interval.min) rec1.t = interval.min;
        if (rec2.t > interval.max) rec2.t = interval.max;

        if (rec1.t >= rec2.t) return false;

        if (rec1.t < 0) rec1.t = 0;

        auto ray_length = ray.dir.length();
        auto distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
        auto hit_distance = neg_inv_density_ * log(random_double());

        if (hit_distance > distance_inside_boundary) return false;

        rec.t = rec1.t + hit_distance / ray_length;
        rec.p = ray.at(rec.t);

        rec.normal = Vec3(1, 0, 0);
        rec.is_front_face = true;
        rec.mat = mat_;

        return true;
    }

    AABB bounding_box() const override { return boundary_->bounding_box(); }

private:
    std::shared_ptr<Hittable> boundary_;
    double neg_inv_density_;
    std::shared_ptr<Material> mat_;
};

}  // namespace cray