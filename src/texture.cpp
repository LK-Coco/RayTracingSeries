#include "texture.h"

namespace cray {

Color CheckerTex::value(double u, double v, const Point3& p) const {
    int x_int = static_cast<int>(std::floor(inv_scale * p.x));
    int y_int = static_cast<int>(std::floor(inv_scale * p.y));
    int z_int = static_cast<int>(std::floor(inv_scale * p.z));

    bool is_even = (x_int + y_int + z_int) % 2 == 0;
    return is_even ? even->value(u, v, p) : odd->value(u, v, p);
}

}  // namespace cray