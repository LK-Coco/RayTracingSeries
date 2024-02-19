#pragma once

#include "hittable.h"

namespace cray {

class Camera {
public:
    // void render(const Hittable& world, std::ostream& out);

    void render_to_png(const Hittable& world, const char* file_name);

    int image_width = 100;
    double aspect_ratio = 1.0;

    double fov = 90;  // 垂直方向的fov
    Point3 position = Point3(0.0, 0.0, -1.0);
    Point3 look_at = Point3(0.0, 0.0, 0.0);
    Vec3 up = Vec3(0.0, 1.0, 0.0);

    double focus_dist = 10.0;  // 焦距
    double defocus_angle = 0.0;

    int samples_per_pixel = 10;
    int max_depth = 10;

    Color background;  // 背景颜色

private:
    void init();

    Vec3 pixel_sample_square() const;

    Ray get_ray(int i, int j) const;

    Color ray_color(const Ray& ray, const Hittable& world, int depth) const;

    Point3 defocus_disk_sample() const;

    int image_height;
    Point3 center;       // Camera center
    Point3 pixel00_loc;  // Location of pixel 0, 0
    Vec3 pixel_delta_u;  // Offset to pixel to the right
    Vec3 pixel_delta_v;  // Offset to pixel below

    Vec3 u, v, w;  // 相机空间的三个轴

    // 光圈平面空间的u、v轴上的长度为光圈半径的向量
    Vec3 defocus_disk_u;
    Vec3 defocus_disk_v;
};

}  // namespace cray