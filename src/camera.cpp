#include "camera.h"
#include "material.h"
#include <iostream>

namespace cray {

double linear_to_gamma(double linear_comp) { return sqrt(linear_comp); }

void write_color(std::ostream& out, Color pixel_color, double scale) {
    static const Interval cl(0.000, 0.999);

    auto r = linear_to_gamma(pixel_color.r * scale);
    auto g = linear_to_gamma(pixel_color.g * scale);
    auto b = linear_to_gamma(pixel_color.b * scale);

    out << static_cast<int>(255.999 * cl.clamp(r)) << ' '
        << static_cast<int>(255.999 * cl.clamp(g)) << ' '
        << static_cast<int>(255.999 * cl.clamp(b)) << '\n';
}

void Camera::render(const Hittable& world, std::ostream& out) {
    init();

    out << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    const auto scale = 1.0 / samples_per_pixel;

    for (int j = 0; j < image_height; ++j) {
        for (int i = 0; i < image_width; ++i) {
            Color pixel_color(0, 0, 0);
            for (int sample = 0; sample < samples_per_pixel; ++sample) {
                auto ray = get_ray(i, j);
                pixel_color += ray_color(ray, world, max_depth);
            }

            write_color(out, pixel_color, scale);
        }
    }
}

void Camera::init() {
    image_height = static_cast<int>(image_width / aspect_ratio);
    image_height = image_height < 1 ? 1 : image_height;

    center = position;

    // 视口长宽
    auto theta = degrees_to_radians(fov);
    auto h = tan(theta / 2);
    auto viewport_height = 2.0 * h * focus_dist;
    auto viewport_width =
        viewport_height * (static_cast<double>(image_width) / image_height);

    w = unit_vector(position - look_at);
    u = unit_vector(cross(up, w));
    v = unit_vector(cross(w, u));

    // 视口的uv表示
    auto viewport_u = viewport_width * u;
    auto viewport_v = -viewport_height * v;

    // 像素的长宽delta
    pixel_delta_u = viewport_u / image_width;
    pixel_delta_v = viewport_v / image_height;

    auto viewport_upper_left =
        center - focus_dist * w - viewport_u / 2 - viewport_v / 2;
    pixel00_loc = viewport_upper_left + 0.5f * (pixel_delta_u + pixel_delta_v);

    auto defocus_radius =
        focus_dist * tan(degrees_to_radians(defocus_angle / 2));
    defocus_disk_u = u * defocus_radius;
    defocus_disk_v = v * defocus_radius;
}

Color Camera::ray_color(const Ray& ray, const Hittable& world,
                        int depth) const {
    if (depth <= 0) {
        return Color(0, 0, 0);
    }

    HitRecord rec;

    if (world.hit(ray, Interval(0.001, Infinity), rec)) {
        Color attenuation;
        Ray scattered_ray;
        if (rec.mat != nullptr &&
            rec.mat->scatter(ray, rec, attenuation, scattered_ray))
            return attenuation * ray_color(scattered_ray, world, depth - 1);
        return Color(0, 0, 0);
    }

    Vec3 unit_direction = unit_vector(ray.dir);
    auto a = 0.5 * (unit_direction.y + 1.0);
    return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
}

Vec3 Camera::pixel_sample_square() const {
    auto px = -0.5 + random_double();
    auto py = -0.5 + random_double();
    return (px * pixel_delta_u) + (py * pixel_delta_v);
}

Ray Camera::get_ray(int i, int j) const {
    auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
    auto pixel_sample = pixel_center + pixel_sample_square();

    auto ray_origin = defocus_angle <= 0 ? center : defocus_disk_sample();
    auto ray_direction = pixel_sample - ray_origin;

    auto ray_time = random_double();

    return Ray(ray_origin, ray_direction, ray_time);
}

Point3 Camera::defocus_disk_sample() const {
    // 返回光圈平面内的随机一个点
    auto p = random_in_unit_disk();
    return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
}

}  // namespace cray