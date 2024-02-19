#pragma once

#include "hit_record.h"
#include "texture.h"

namespace cray {

struct Material {
    virtual ~Material() = default;

    virtual bool scatter(const Ray& r_in, const HitRecord& rec,
                         Color& attenuation, Ray& scattered) const = 0;

    virtual Color emitted(double u, double v, const Point3& p) const {
        return Color(0, 0, 0);
    }
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

struct DiffuseLight : public Material {
    DiffuseLight(std::shared_ptr<Texture> a) : emit(a) {}
    DiffuseLight(Color c) : emit(std::make_shared<SolidColorTex>(c)) {}

    bool scatter(const Ray& r_in, const HitRecord& rec, Color& attenuation,
                 Ray& scattered) const override {
        return false;
    }

    Color emitted(double u, double v, const Point3& p) const override {
        return emit->value(u, v, p);
    }

    std::shared_ptr<Texture> emit;
};

// 各向同性
struct Isotropic : public Material {
    Isotropic(Color c) : albedo(std::make_shared<SolidColorTex>(c)) {}
    Isotropic(std::shared_ptr<Texture> tex) : albedo(tex) {}

    bool scatter(const Ray& r_in, const HitRecord& rec, Color& attenuation,
                 Ray& scattered) const override {
        scattered = Ray(rec.p, random_unit_vector(), r_in.tm);
        attenuation = albedo->value(rec.u, rec.v, rec.p);
        return true;
    }

    std::shared_ptr<Texture> albedo;
};

}  // namespace cray