#include "material.h"

namespace cray {

bool Lambertian::scatter(const Ray& r_in, const HitRecord& rec,
                         Color& attenuation, Ray& scattered) const {
    auto scatter_direction = rec.normal + random_unit_vector();
    if (scatter_direction.near_zero()) {
        scatter_direction = rec.normal;
    }
    scattered = Ray(rec.p, scatter_direction, r_in.tm);
    attenuation = albedo->value(rec.u, rec.v, rec.p);
    return true;
}

bool Metal::scatter(const Ray& r_in, const HitRecord& rec, Color& attenuation,
                    Ray& scattered) const {
    auto scatter_dir = reflect(unit_vector(r_in.dir), rec.normal);
    scattered = Ray(rec.p, scatter_dir + fuzz * random_unit_vector(), r_in.tm);

    attenuation = albedo;
    return dot(scattered.dir, rec.normal) > 0;
}

// 根据视角（入射光）计算反射比，即菲涅尔现象
double reflectance(double cosine, double ref_idx) {
    // Use Schlick's approximation for reflectance.
    auto r0 = (1 - ref_idx) / (1 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
}

bool Dielectric::scatter(const Ray& r_in, const HitRecord& rec,
                         Color& attenuation, Ray& scattered) const {
    attenuation = Color(1, 1, 1);
    double refract_ratio = rec.is_front_face ? (1.0 / ir) : ir;
    auto r_in_dir_uint = unit_vector(r_in.dir);

    auto cos_theta = fmin(dot(-r_in_dir_uint, rec.normal), 1.0);
    auto sin_theta = sqrt(1 - cos_theta * cos_theta);

    bool can_refract = refract_ratio * sin_theta <= 1.0;
    Vec3 scatter_dir;
    if (can_refract && reflectance(cos_theta, refract_ratio) < random_double())
        scatter_dir = refract(r_in_dir_uint, rec.normal, refract_ratio);
    else
        scatter_dir = reflect(r_in_dir_uint, rec.normal);
    scattered = Ray(rec.p, scatter_dir, r_in.tm);
    return true;
}

}  // namespace cray