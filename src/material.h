#pragma once

#include "hit_record.h"
#include "texture.h"

namespace cray {

struct Material {
    virtual ~Material() = default;

    virtual bool scatter(const Ray& r_in, const HitRecord& rec,
                         Color& attenuation, Ray& scattered) const = 0;
};

struct Lambertian : public Material {
    Lambertian(const Color& c) : albedo(std::make_shared<SolidColorTex>(c)) {}
    Lambertian(std::shared_ptr<Texture> tex) : albedo(tex) {}

    bool scatter(const Ray& r_in, const HitRecord& rec, Color& attenuation,
                 Ray& scattered) const override;

    std::shared_ptr<Texture> albedo;
};

struct Metal : public Material {
    Metal(const Color& c, double f) : albedo(c), fuzz(f) {}

    bool scatter(const Ray& r_in, const HitRecord& rec, Color& attenuation,
                 Ray& scattered) const override;

    Color albedo;
    double fuzz;  // 粗糙程度
};

// 电介质，绝缘体
struct Dielectric : public Material {
    Dielectric(double index_of_refraction) : ir(index_of_refraction) {}

    bool scatter(const Ray& r_in, const HitRecord& rec, Color& attenuation,
                 Ray& scattered) const override;

    double ir;
};

}  // namespace cray