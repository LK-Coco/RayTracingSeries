#include <iostream>
#include <fstream>
#include "material.h"
#include "camera.h"
#include "shapes.h"
#include "hittable_list.h"
#include "bvh.h"
#include "texture.h"

using namespace cray;

std::shared_ptr<HittableList> box(const Point3& a, const Point3& b,
                                  std::shared_ptr<Material> mat) {
    auto sides = std::make_shared<HittableList>();

    auto min = Point3(fmin(a.x, b.x), fmin(a.y, b.y), fmin(a.z, b.z));
    auto max = Point3(fmax(a.x, b.x), fmax(a.y, b.y), fmax(a.z, b.z));

    auto dx = Vec3(max.x - min.x, 0, 0);
    auto dy = Vec3(0, max.y - min.y, 0);
    auto dz = Vec3(0, 0, max.z - min.z);

    sides->add(std::make_shared<Quad>(Point3(min.x, min.y, max.z), dx, dy,
                                      mat));  // front
    sides->add(std::make_shared<Quad>(Point3(max.x, min.y, max.z), -dz, dy,
                                      mat));  // right
    sides->add(std::make_shared<Quad>(Point3(max.x, min.y, min.z), -dx, dy,
                                      mat));  // back
    sides->add(std::make_shared<Quad>(Point3(min.x, min.y, min.z), dz, dy,
                                      mat));  // left
    sides->add(std::make_shared<Quad>(Point3(min.x, max.y, max.z), dx, -dz,
                                      mat));  // top
    sides->add(std::make_shared<Quad>(Point3(min.x, min.y, min.z), dx, dz,
                                      mat));  // bottom

    return sides;
}

void render_book1_world(int width, int per_sample, int max_depth) {
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
    cam.image_width = width;
    cam.aspect_ratio = 16.0 / 9.0;
    cam.samples_per_pixel = per_sample;
    cam.max_depth = max_depth;

    cam.fov = 20;
    cam.position = Point3(13, 2, 3);
    cam.look_at = Point3(0, 0, 0);
    cam.up = Vec3(0, 1, 0);

    cam.defocus_angle = 0.6;
    cam.focus_dist = 10.0;

    cam.background = Color(0.70, 0.80, 1.00);

    cam.render_to_png(world, "data/book01.png");
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

    cam.background = Color(0.70, 0.80, 1.00);

    cam.render_to_png(HittableList(globe), "data/earth.png");
}

void render_noise() {
    HittableList world;

    auto pertext = std::make_shared<NoiseTex>(4);
    world.add(std::make_shared<Sphere>(Point3(0, -1000, 0), 1000,
                                       make_shared<Lambertian>(pertext)));
    world.add(std::make_shared<Sphere>(Point3(0, 2, 0), 2,
                                       make_shared<Lambertian>(pertext)));

    Camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;

    cam.fov = 20;
    cam.position = Point3(13, 2, 3);
    cam.look_at = Point3(0, 0, 0);
    cam.up = Vec3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.background = Color(0.70, 0.80, 1.00);

    cam.render_to_png(world, "data/noise.png");
}

void render_quads() {
    HittableList world;

    // Materials
    auto left_red = std::make_shared<Lambertian>(Color(1.0, 0.2, 0.2));
    auto back_green = std::make_shared<Lambertian>(Color(0.2, 1.0, 0.2));
    auto right_blue = std::make_shared<Lambertian>(Color(0.2, 0.2, 1.0));
    auto upper_orange = std::make_shared<Lambertian>(Color(1.0, 0.5, 0.0));
    auto lower_teal = std::make_shared<Lambertian>(Color(0.2, 0.8, 0.8));

    // Quads
    world.add(std::make_shared<Quad>(Point3(-3, -2, 5), Vec3(0, 0, -4),
                                     Vec3(0, 4, 0), left_red));
    world.add(std::make_shared<Quad>(Point3(-2, -2, 0), Vec3(4, 0, 0),
                                     Vec3(0, 4, 0), back_green));
    world.add(std::make_shared<Quad>(Point3(3, -2, 1), Vec3(0, 0, 4),
                                     Vec3(0, 4, 0), right_blue));
    world.add(std::make_shared<Quad>(Point3(-2, 3, 1), Vec3(4, 0, 0),
                                     Vec3(0, 0, 4), upper_orange));
    world.add(std::make_shared<Quad>(Point3(-2, -3, 5), Vec3(4, 0, 0),
                                     Vec3(0, 0, -4), lower_teal));

    Camera cam;

    cam.aspect_ratio = 1.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;

    cam.fov = 80;
    cam.position = Point3(0, 0, 9);
    cam.look_at = Point3(0, 0, 0);
    cam.up = Vec3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.background = Color(0.70, 0.80, 1.00);

    cam.render_to_png(world, "data/quad.png");
}

void render_simple_light() {
    HittableList world;

    auto pertext = std::make_shared<NoiseTex>(4);
    world.add(std::make_shared<Sphere>(Point3(0, -1000, 0), 1000,
                                       make_shared<Lambertian>(pertext)));
    world.add(std::make_shared<Sphere>(Point3(0, 2, 0), 2,
                                       make_shared<Lambertian>(pertext)));

    auto difflight = std::make_shared<DiffuseLight>(Color(4, 4, 4));
    world.add(std::make_shared<Quad>(Point3(3, 1, -2), Vec3(2, 0, 0),
                                     Vec3(0, 2, 0), difflight));

    Camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;
    cam.background = Color(0, 0, 0);

    cam.fov = 20;
    cam.position = Point3(26, 3, 6);
    cam.look_at = Point3(0, 2, 0);
    cam.up = Vec3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.render_to_png(world, "data/simple_light.png");
}

void render_cornell_box() {
    HittableList world;

    auto red = std::make_shared<Lambertian>(Color(.65, .05, .05));
    auto white = std::make_shared<Lambertian>(Color(.73, .73, .73));
    auto green = std::make_shared<Lambertian>(Color(.12, .45, .15));
    auto light = std::make_shared<DiffuseLight>(Color(15, 15, 15));

    world.add(std::make_shared<Quad>(Point3(555, 0, 0), Vec3(0, 555, 0),
                                     Vec3(0, 0, 555), green));
    world.add(std::make_shared<Quad>(Point3(0, 0, 0), Vec3(0, 555, 0),
                                     Vec3(0, 0, 555), red));
    world.add(std::make_shared<Quad>(Point3(343, 554, 332), Vec3(-130, 0, 0),
                                     Vec3(0, 0, -105), light));
    world.add(std::make_shared<Quad>(Point3(0, 0, 0), Vec3(555, 0, 0),
                                     Vec3(0, 0, 555), white));
    world.add(std::make_shared<Quad>(Point3(555, 555, 555), Vec3(-555, 0, 0),
                                     Vec3(0, 0, -555), white));
    world.add(std::make_shared<Quad>(Point3(0, 0, 555), Vec3(555, 0, 0),
                                     Vec3(0, 555, 0), white));

    std::shared_ptr<Hittable> box1 =
        box(Point3(0, 0, 0), Point3(165, 330, 165), white);
    box1 = std::make_shared<RotateY>(box1, 15);
    box1 = std::make_shared<Translate>(box1, Vec3(265, 0, 295));
    world.add(box1);

    std::shared_ptr<Hittable> box2 =
        box(Point3(0, 0, 0), Point3(165, 165, 165), white);
    box2 = std::make_shared<RotateY>(box2, -18);
    box2 = std::make_shared<Translate>(box2, Vec3(130, 0, 65));
    world.add(box2);

    Camera cam;

    cam.aspect_ratio = 1.0;
    cam.image_width = 600;
    cam.samples_per_pixel = 200;
    cam.max_depth = 50;
    cam.background = Color(0, 0, 0);

    cam.fov = 40;
    cam.position = Point3(278, 278, -800);
    cam.look_at = Point3(278, 278, 0);
    cam.up = Vec3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.render_to_png(world, "data/cornell_box.png");
}

void render_cornell_smoke() {
    HittableList world;

    auto red = std::make_shared<Lambertian>(Color(.65, .05, .05));
    auto white = std::make_shared<Lambertian>(Color(.73, .73, .73));
    auto green = std::make_shared<Lambertian>(Color(.12, .45, .15));
    auto light = std::make_shared<DiffuseLight>(Color(7, 7, 7));

    world.add(std::make_shared<Quad>(Point3(555, 0, 0), Vec3(0, 555, 0),
                                     Vec3(0, 0, 555), green));
    world.add(std::make_shared<Quad>(Point3(0, 0, 0), Vec3(0, 555, 0),
                                     Vec3(0, 0, 555), red));
    world.add(std::make_shared<Quad>(Point3(113, 554, 127), Vec3(330, 0, 0),
                                     Vec3(0, 0, 305), light));
    world.add(std::make_shared<Quad>(Point3(0, 555, 0), Vec3(555, 0, 0),
                                     Vec3(0, 0, 555), white));
    world.add(std::make_shared<Quad>(Point3(0, 0, 0), Vec3(555, 0, 0),
                                     Vec3(0, 0, 555), white));
    world.add(std::make_shared<Quad>(Point3(0, 0, 555), Vec3(555, 0, 0),
                                     Vec3(0, 555, 0), white));

    std::shared_ptr<Hittable> box1 =
        box(Point3(0, 0, 0), Point3(165, 330, 165), white);
    box1 = std::make_shared<RotateY>(box1, 15);
    box1 = std::make_shared<Translate>(box1, Vec3(265, 0, 295));

    std::shared_ptr<Hittable> box2 =
        box(Point3(0, 0, 0), Point3(165, 165, 165), white);
    box2 = std::make_shared<RotateY>(box2, -18);
    box2 = std::make_shared<Translate>(box2, Vec3(130, 0, 65));

    world.add(std::make_shared<ConstantMedium>(box1, 0.01, Color(0, 0, 0)));
    world.add(std::make_shared<ConstantMedium>(box2, 0.01, Color(1, 1, 1)));

    Camera cam;

    cam.aspect_ratio = 1.0;
    cam.image_width = 600;
    cam.samples_per_pixel = 200;
    cam.max_depth = 50;
    cam.background = Color(0, 0, 0);

    cam.fov = 40;
    cam.position = Point3(278, 278, -800);
    cam.look_at = Point3(278, 278, 0);
    cam.up = Vec3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.render_to_png(world, "data/cornell_smoke.png");
}

void render_book2_scene(int image_width, int samples_per_pixel, int max_depth) {
    HittableList boxes1;
    auto ground = std::make_shared<Lambertian>(Color(0.48, 0.83, 0.53));

    int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++) {
        for (int j = 0; j < boxes_per_side; j++) {
            auto w = 100.0;
            auto x0 = -1000.0 + i * w;
            auto z0 = -1000.0 + j * w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = random_double(1, 101);
            auto z1 = z0 + w;

            boxes1.add(box(Point3(x0, y0, z0), Point3(x1, y1, z1), ground));
        }
    }

    HittableList world;

    world.add(std::make_shared<BVHNode>(boxes1));

    auto light = std::make_shared<DiffuseLight>(Color(7, 7, 7));
    world.add(std::make_shared<Quad>(Point3(123, 554, 147), Vec3(300, 0, 0),
                                     Vec3(0, 0, 265), light));

    auto center1 = Point3(400, 400, 200);
    auto center2 = center1 + Vec3(30, 0, 0);
    auto sphere_material = std::make_shared<Lambertian>(Color(0.7, 0.3, 0.1));
    world.add(std::make_shared<Sphere>(center1, center2, 50, sphere_material));

    world.add(std::make_shared<Sphere>(Point3(260, 150, 45), 50,
                                       std::make_shared<Dielectric>(1.5)));
    world.add(std::make_shared<Sphere>(
        Point3(0, 150, 145), 50,
        std::make_shared<Metal>(Color(0.8, 0.8, 0.9), 1.0)));

    auto boundary = std::make_shared<Sphere>(Point3(360, 150, 145), 70,
                                             std::make_shared<Dielectric>(1.5));
    world.add(boundary);
    world.add(
        std::make_shared<ConstantMedium>(boundary, 0.2, Color(0.2, 0.4, 0.9)));
    boundary = std::make_shared<Sphere>(Point3(0, 0, 0), 5000,
                                        std::make_shared<Dielectric>(1.5));
    world.add(
        std::make_shared<ConstantMedium>(boundary, .0001, Color(1, 1, 1)));

    auto emat = std::make_shared<Lambertian>(
        std::make_shared<ImageTex>("data/earthmap.jpg"));
    world.add(std::make_shared<Sphere>(Point3(400, 200, 400), 100, emat));
    auto pertext = std::make_shared<NoiseTex>(0.1);
    world.add(std::make_shared<Sphere>(Point3(220, 280, 300), 80,
                                       std::make_shared<Lambertian>(pertext)));

    HittableList boxes2;
    auto white = std::make_shared<Lambertian>(Color(.73, .73, .73));
    int ns = 1000;
    for (int j = 0; j < ns; j++) {
        boxes2.add(std::make_shared<Sphere>(Point3::random(0, 165), 10, white));
    }

    world.add(std::make_shared<Translate>(
        std::make_shared<RotateY>(std::make_shared<BVHNode>(boxes2), 15),
        Vec3(-100, 270, 395)));

    Camera cam;

    cam.aspect_ratio = 1.0;
    cam.image_width = image_width;
    cam.samples_per_pixel = samples_per_pixel;
    cam.max_depth = max_depth;
    cam.background = Color(0, 0, 0);

    cam.fov = 40;
    cam.position = Point3(478, 278, -600);
    cam.look_at = Point3(278, 278, 0);
    cam.up = Vec3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.render_to_png(world, "data/book2_scene.png");
}

int main() {
    render_book1_world(1200, 500, 50);
    // render_book2_scene(400, 250, 4);
}