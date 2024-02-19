#pragma once

#include "cgmath.h"
#include "cray_image.h"
#include "perlin.h"

namespace cray {

struct Texture {
    virtual ~Texture() = default;
    virtual Color value(double u, double v, const Point3& p) const = 0;
};

struct SolidColorTex : public Texture {
    SolidColorTex(Color c) : color_val(c) {}
    SolidColorTex(double r, double g, double b) : color_val(Color(r, g, b)) {}

    Color value(double u, double v, const Point3& p) const override {
        return color_val;
    }

    Color color_val;
};

struct CheckerTex : public Texture {
    CheckerTex(double scale, std::shared_ptr<Texture> _even,
               std::shared_ptr<Texture> _odd)
        : inv_scale(1.0 / scale),
          even(_even),
          odd(_odd) {}
    CheckerTex(double scale, Color c0, Color c1)
        : inv_scale(1.0 / scale),
          even(std::make_shared<SolidColorTex>(c0)),
          odd(std::make_shared<SolidColorTex>(c1)) {}

    Color value(double u, double v, const Point3& p) const override;

    double inv_scale;
    std::shared_ptr<Texture> even;
    std::shared_ptr<Texture> odd;
};

struct ImageTex : public Texture {
    ImageTex(const std::string& path) : image(path) {}

    Color value(double u, double v, const Point3& p) const override;

    CRayImage image;
};

struct NoiseTex : public Texture {
    NoiseTex() : scale(1.0) {}
    NoiseTex(double scale_) : scale(scale_) {}

    Color value(double u, double v, const Point3& p) const override {
        // return Color(1, 1, 1) * 0.5 * (1.0 + perlin.noise(scale * p));

        auto s = scale * p;
        return Color(1, 1, 1) * 0.5 * (1 + sin(s.z + 10 * perlin.turb(s)));
    }

    Perlin perlin;
    double scale;
};

}  // namespace cray