#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__
#include <cmath>
#include <cassert>
#include <iostream>

template <size_t DIM> struct vec 
{
    float data[DIM] = {};

    float& operator[](const size_t i) { assert(i < DIM); return data[i]; }
    const float& operator[](const size_t i) const { assert(i < DIM); return data[i]; }

    float length_squared() const {
        float ret = 0;
        for (size_t i = 0; i < DIM; ret += data[i] * data[i], i++);
        return ret;
    }

    float legth() {
        return sqrt(length_squared());
    }
};

template<size_t DIM> vec<DIM> operator*(const vec<DIM>& lhs, const float rhs) 
{
    vec<DIM> ret;
    for (size_t i = DIM; i--; ret[i] = lhs[i] * rhs);
    return ret;
}

template<size_t DIM> float operator*(const vec<DIM>& lhs, const vec<DIM>& rhs) 
{
    float ret = 0;
    for (size_t i = DIM; i--; ret += lhs[i] * rhs[i]);
    return ret;
}

template<size_t DIM> vec<DIM> operator/(const vec<DIM>& lhs, const float rhs) {
    return lhs * (1 / rhs);
}



template<size_t DIM> vec<DIM> operator+(vec<DIM> lhs, const vec<DIM>& rhs) 
{

    for (size_t i = DIM; i--; lhs[i] += rhs[i]);
    return lhs;
}

template<size_t DIM> vec<DIM> operator-(vec<DIM> lhs, const vec<DIM>& rhs) 
{
    for (size_t i = DIM; i--; lhs[i] -= rhs[i]);
    return lhs;
}

template<size_t DIM> vec<DIM> operator-(const vec<DIM>& lhs) 
{
    return lhs * (-1.f);
}



template <> struct vec<3> 
{
    vec<3>() {}
    vec<3>(float x, float y, float z) :x(x), y(y), z(z) {}
    float& operator[](const size_t i) { assert(i < 3); return i == 0 ? x : (1 == i ? y : z); }
    const float& operator[](const size_t i) const { assert(i < 3); return i == 0 ? x : (1 == i ? y : z); }
    /// <summary>
    /// 求向量长度
    /// </summary>
    /// <returns></returns>
    float norm() { return std::sqrt(x * x + y * y + z * z); }
    float length_squared() { return x * x + y * y + z * z; }
    vec<3>& normalize(float l = 1) { *this = (*this) * (l / norm()); return *this; }
    float x = 0, y = 0, z = 0;

    vec<3>& operator+=(const vec<3>& v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    inline static vec<3> random() {
        return vec<3>{random_float(), random_float(), random_float()};
    }

    inline static vec<3> random(float min, float max) {
        return vec<3>{random_float(min, max), random_float(min, max), random_float(min, max)};
    }

    bool near_zero()const {
        const auto s = 1e-8;
        return ((fabs(x) < s) && (fabs(y) < s) && (fabs(z) < s));
    }
};

typedef vec<3> vec3;
typedef vec<3> point3;
typedef vec<3> color;
typedef vec<4> vec4;

inline vec3 operator*(const vec3& u, const vec3 v) { return vec3{ u.x * v.x, u.y * v.y, u.z * v.z }; }

vec3 cross(vec3 v1, vec3 v2)
{
    return { v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x };
}

float dot(const vec3& u, const vec3& v) {
    return u.x * v.x + u.y * v.y + u.z * v.z;
}

vec3 random_in_unit_sphere() {
    while (true) {
        auto p = vec3::random(-1, 1);
        if (p.length_squared() >= 1) continue;
        return p;
    }
}

vec3 random_in_unit_disk() {
    while (true) {
        auto p = vec3(random_float(-1, 1), random_float(-1, 1), 0);
        if (p.length_squared() >= 1) continue;
        return p;
    }
}

vec3 random_unit_vector() {
    return random_in_unit_sphere().normalize();
}

vec3 random_in_hemisphere(const vec3& normal) {
    vec3 in_unit_sphere = random_in_unit_sphere();
    if (dot(in_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
        return in_unit_sphere;
    else
        return -in_unit_sphere;
}

// 反射
vec3 reflect(const vec3& v, const vec3& n) {
    return v - n * 2 * dot(v, n);
}

// 折射
vec3 refract(const vec3& uv, const vec3& n, float etai_over_etat) {
    auto cos_theta = fmin(dot(-uv, n), 1.0f);
    vec3 r_out_perp =  (uv + n* cos_theta) * etai_over_etat;
    vec3 r_out_parallel = -n*sqrt(fabs(1.0f - r_out_perp.length_squared()));
    return r_out_perp + r_out_parallel;
}


template <size_t DIM> std::ostream& operator<<(std::ostream& out, const vec<DIM>& v) 
{
    for (size_t i = 0; i < DIM; i++)
        out << v[i] << " ";
    return out;
}

#endif //__GEOMETRY_H__