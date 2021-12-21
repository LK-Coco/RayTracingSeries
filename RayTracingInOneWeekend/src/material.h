#ifndef MATERIAL_H
#define MATERIAL_H

#include "rtweekend.h"
#include "hittable.h"



class material {
public:
	virtual bool scatter(const ray& r_in,const hit_record&rec,color& attenuation,ray& scattered) const = 0;
};


class lambertian :public material {
public:
	color albedo;
public:
	lambertian(const color& a):albedo(a) {}

	virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
		auto scatter_direction = rec.normal + random_unit_vector();
		if (scatter_direction.near_zero())
			scatter_direction = rec.normal;
		scattered = ray(rec.p, scatter_direction);
		attenuation = albedo;
		return true;
	}
};

class metal :public material {
public:
	color albedo;
	float fuzz; // 控制毛玻璃的程度
public:
	metal(const color& a,float f) :albedo(a),fuzz(f < 1? f:1) {}

	virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
		vec3 reflected = reflect(r_in.direction().normalize(), rec.normal);
		scattered = ray(rec.p, reflected+random_in_unit_sphere()*fuzz);
		attenuation = albedo;
		return (dot(scattered.direction(), rec.normal) > 0);
	}
};

class dielectric :public material {
public:
	float ir;
public:
	dielectric(float index_of_refraction) :ir(index_of_refraction) {}

	virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
		attenuation = color{1.0f, 1.0f, 1.0f};
		float refraction_ratio = rec.front_face ? (1.0f / ir) : ir;

		vec3 unit_direction = r_in.direction().normalize();
		float cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0f);
		float sin_theta = sqrt(1.0f - cos_theta * cos_theta);

		bool cannot_refract = refraction_ratio * sin_theta > 1.0f;
		vec3 direction;

		if (cannot_refract || reflectance(cos_theta,refraction_ratio) > random_float())
			direction = reflect(unit_direction, rec.normal);
		else
			direction = refract(unit_direction, rec.normal, refraction_ratio);

		scattered = ray(rec.p, direction);

		return true;
	}

private:
	static float reflectance(float cosine, float ref_idx) {
		// Use Schlick's approximation for reflectance.
		auto r0 = (1.f - ref_idx) / (1.f + ref_idx);
		r0 = r0 * r0;
		return r0 + (1.f - r0) * pow((1.f - cosine), 5);
	}
};

#endif