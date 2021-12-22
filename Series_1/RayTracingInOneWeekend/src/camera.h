#ifndef CAMERA_H
#define CAMERA_H

#include "rtweekend.h"

class camera {
private:
	point3 origin;
	point3 lower_left_corner;
	vec3 horizontal;
	vec3 vertical;
	vec3 u, v, w;
	float lens_radius;
public:
	camera(point3 lookfrom,point3 lookat,vec3 vup,float vfov,float aspect_ratio,
		float aperture,float focus_dist) {
		auto theta = degrees_to_radians(vfov);
		auto h = tan(theta / 2.f);
		auto viewport_height = 2.0f * h;
		auto viewport_width = aspect_ratio * viewport_height;

		w = (lookfrom - lookat).normalize(); // 相机平面法线
		u = cross(vup, w).normalize();
		v = cross(w, u);


		origin = lookfrom;
		horizontal = u*viewport_width * focus_dist;
		vertical = v*viewport_height*focus_dist;
		lower_left_corner = origin - horizontal / 2.f - vertical / 2.f - w* focus_dist;

		lens_radius = aperture / 2.f;
	}

	ray get_ray(float s, float t) const {
		vec3 rd = random_in_unit_disk() * lens_radius;
		vec3 offset = u * rd.x + v * rd.y;

		return ray(
			origin + offset,
			lower_left_corner +  horizontal*s + vertical*t - origin - offset
		);
	}
};


#endif