#pragma once

#include <array>
#include "common.h"
#include "cgmath.h"

namespace cray {

template <int COUNT>
constexpr auto perlin_generate_perm() {
    auto p = std::array<int, COUNT>();
    for (uint32_t i = 0; i < COUNT; ++i) {
        p[i] = i;
    }
    for (uint32_t i = COUNT - 1; i > 0; --i) {
        int target = random_int(0, i);
        std::swap(p[i], p[target]);
        // int tmp = p[i];
        // p[i] = p[target];
        // p[target] = tmp;
    }

    return p;
}

// constexpr auto permx = perlin_generate_perm<256>();

class Perlin {
public:
    Perlin() {
        random_vec_ = new Vec3[POINT_COUNT];
        for (uint32_t i = 0; i < POINT_COUNT; ++i) {
            random_vec_[i] = unit_vector(Vec3::random(-1, 1));
        }

        perm_x_ = perlin_generate_perm();
        perm_y_ = perlin_generate_perm();
        perm_z_ = perlin_generate_perm();
    }

    ~Perlin() {
        delete[] random_vec_;
        delete[] perm_x_;
        delete[] perm_y_;
        delete[] perm_z_;
    }

    double noise(const Point3& p) const {
        auto u = p.x - floor(p.x);
        auto v = p.y - floor(p.y);
        auto w = p.z - floor(p.z);

        auto i = static_cast<int>(floor(p.x));
        auto j = static_cast<int>(floor(p.y));
        auto k = static_cast<int>(floor(p.z));
        Vec3 c[2][2][2];
        for (int di = 0; di < 2; di++) {
            for (int dj = 0; dj < 2; dj++) {
                for (int dk = 0; dk < 2; dk++) {
                    c[di][dj][dk] = random_vec_[perm_x_[(i + di) & 255] ^
                                                perm_y_[(j + dj) & 255] ^
                                                perm_z_[(k + dk) & 255]];
                }
            }
        }

        // trilinear_interp
        auto uu = u * u * (3 - 2 * u);
        auto vv = v * v * (3 - 2 * v);
        auto ww = w * w * (3 - 2 * w);
        auto accum = 0.0;
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    Vec3 weight_v(u - i, v - j, w - k);
                    accum += (i * uu + (1 - i) * (1 - uu)) *
                             (j * vv + (1 - j) * (1 - vv)) *
                             (k * ww + (1 - k) * (1 - ww)) *
                             dot(c[i][j][k], weight_v);
                }
            }
        }

        return accum;
    }

    double turb(const Point3& p, int depth = 7) const {
        auto accum = 0.0;
        auto temp_p = p;
        auto weight = 1.0;

        for (int i = 0; i < depth; i++) {
            accum += weight * noise(temp_p);
            weight *= 0.5;
            temp_p *= 2;
        }

        return fabs(accum);
    }

private:
    static const int POINT_COUNT = 256;
    Vec3* random_vec_;
    int* perm_x_;
    int* perm_y_;
    int* perm_z_;

    static int* perlin_generate_perm() {
        auto p = new int[POINT_COUNT];
        for (uint32_t i = 0; i < POINT_COUNT; ++i) {
            p[i] = i;
        }
        for (uint32_t i = POINT_COUNT - 1; i > 0; --i) {
            int target = random_int(0, i);
            std::swap(p[i], p[target]);
            // int tmp = p[i];
            // p[i] = p[target];
            // p[target] = tmp;
        }

        return p;
    }
};

}  // namespace cray