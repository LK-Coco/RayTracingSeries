#include "texture.h"
#include "interval.h"
namespace cray {

Color CheckerTex::value(double u, double v, const Point3& p) const {
    int x_int = static_cast<int>(std::floor(inv_scale * p.x));
    int y_int = static_cast<int>(std::floor(inv_scale * p.y));
    int z_int = static_cast<int>(std::floor(inv_scale * p.z));

    bool is_even = (x_int + y_int + z_int) % 2 == 0;
    return is_even ? even->value(u, v, p) : odd->value(u, v, p);
}

Color ImageTex::value(double u, double v, const Point3& p) const {
    if (image.invalid()) return Color(0, 1, 1);

    u = Interval(0, 1).clamp(u);
    v = 1.0 - Interval(0, 1).clamp(v);

    auto i = static_cast<int>(u * image.width());
    auto j = static_cast<int>(v * image.height());
    auto pixel = image.pixel_data(i, j);

    const auto color_scale = 1.0 / 255.0;
    return Color(color_scale * pixel[0], color_scale * pixel[1],
                 color_scale * pixel[2]);
}

}  // namespace cray