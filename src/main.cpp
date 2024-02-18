#include <iostream>
#include <fstream>
#include "material.h"
#include "camera.h"
#include "shapes.h"
#include "hittable_list.h"
#include "bvh.h"
#include "texture.h"

using namespace cray;

void render_example_world() {
    HittableList world;

    auto tex = std::make_shared<CheckerTex>(0.32, Color(.2, .3, .1),
                                            Color(.9, .9, .9));

    auto ground_material = std::make_shared<Lambertian>(tex);
    world.add(
        std::make_shared<Sphere>(Point3(0, -1000, 0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            Point3 center(a + 0.9 * random_double(), 0.2,
                          b + 0.9 * random_double());

            if ((center - Point3(4, 0.2, 0)).length() > 0.9) {
                std::shared_ptr<Material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = Color::random() * Color::random();
                    sphere_material = std::make_shared<Lambertian>(albedo);
                    world.add(
                        std::make_shared<Sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = Color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = std::make_shared<Metal>(albedo, fuzz);
                    world.add(
                        std::make_shared<Sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = std::make_shared<Dielectric>(1.5);
                    world.add(
                        std::make_shared<Sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = std::make_shared<Dielectric>(1.5);
    world.add(std::make_shared<Sphere>(Point3(0, 1, 0), 1.0, material1));

    auto material2 = std::make_shared<Lambertian>(Color(0.4, 0.2, 0.1));
    world.add(std::make_shared<Sphere>(Point3(-4, 1, 0), 1.0, material2));

    auto material3 = std::make_shared<Metal>(Color(0.7, 0.6, 0.5), 0.0);
    world.add(std::make_shared<Sphere>(Point3(4, 1, 0), 1.0, material3));

    world = HittableList(std::make_shared<BVHNode>(world));

    Camera cam;
    cam.image_width = 200;  // 1200;
    cam.aspect_ratio = 16.0 / 9.0;
    cam.samples_per_pixel = 50;  // 500;
    cam.max_depth = 10;          // 50;

    cam.fov = 20;
    cam.position = Point3(13, 2, 3);
    cam.look_at = Point3(0, 0, 0);
    cam.up = Vec3(0, 1, 0);

    cam.defocus_angle = 0.6;
    cam.focus_dist = 10.0;

    std::ofstream file;
    file.open("example.ppm", std::ios::out | std::ios::trunc);

    cam.render(world, file);
}

void render_earth() {
    std::string path = "data/earthmap.jpg";
    auto earth_texture = std::make_shared<ImageTex>(path);
    auto earth_surface = std::make_shared<Lambertian>(earth_texture);
    auto globe = std::make_shared<Sphere>(Point3(0, 0, 0), 2, earth_surface);

    Camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 500;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;

    cam.fov = 20;
    cam.position = Point3(0, 0, 12);
    cam.look_at = Point3(0, 0, 0);
    cam.up = Vec3(0, 1, 0);

    cam.defocus_angle = 0;

    std::ofstream file;
    file.open("earth.ppm", std::ios::out | std::ios::trunc);
    cam.render(HittableList(globe), file);
}

int main() { render_earth(); }