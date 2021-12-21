#pragma once
#include <iostream>
#include "geometry.h"

void write_color(std::ostream& out, vec3 pixel_color,int samples_per_pixel) {
	auto r = pixel_color.x;
	auto g = pixel_color.y;
	auto b = pixel_color.z;
	
	// use gamma 2.0
	auto scale = 1.f / (float)samples_per_pixel;
	r = sqrt(r * scale);
	g = sqrt(g * scale);
	b = sqrt(b * scale);

	out << static_cast<int>(256 * clamp(r, 0.f, 0.999f)) << ' '
		<< static_cast<int>(256 * clamp(g, 0.f, 0.999f)) << ' '
		<< static_cast<int>(256 * clamp(b, 0.f, 0.999f)) << '\n';
}