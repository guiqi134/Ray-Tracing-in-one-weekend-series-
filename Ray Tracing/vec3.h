#ifndef VEC3_H
#define VEC3_H

#include "rtweekend.h"

#include <cmath>
#include <iostream>

using std::sqrt;
using std::fabs;

class vec3 
{
    public:
        double e[3]; // stand for (x, y, z)
    public:
        vec3() : e{0, 0, 0} { } // default constr.
        vec3(double e0, double e1, double e2) : e{e0, e1, e2} { } // param. constr.

        // methods
        double x() const { return e[0]; }
        double y() const { return e[1]; }
        double z() const { return e[2]; }
        
        double length() const 
        {
            return sqrt(length_squared());
        }

        double length_squared() const 
        {
            return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
        }

        // Return true if the vector is close to zero in all dimensions.
        bool near_zero() const
        {
            const auto s = 1e-8;
            return (fabs(e[0]) < s) && (fabs(e[1]) < s) && (fabs(e[2]) < s);
        }

        inline static vec3 random()
        {
            return vec3(random_double(), random_double(), random_double());
        }

        inline static vec3 random(double min, double max)
        {
            return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
        }

        // operator overload
        vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
        double operator[](int i) const { return e[i]; }
        double &operator[](int i) { return e[i]; }

        vec3& operator+=(const vec3 &v)
        {
            e[0] += v.e[0];
            e[1] += v.e[1];
            e[2] += v.e[2];
            return *this;
        }

        vec3 &operator*=(const double t)
        {
            e[0] *= t;
            e[1] *= t;
            e[2] *= t;
            return *this;
        }

        vec3 &operator/=(const double t)
        {
            return *this *= 1 / t;
        }
};

// Type aliases for vec3
using point3 = vec3;
using color = vec3;

// vec3 utility functions: for binary operators
inline std::ostream& operator<<(std::ostream &out, const vec3 &v)
{
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline vec3 operator+(const vec3 &u, const vec3 &v)
{
    return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline vec3 operator-(const vec3 &u, const vec3 &v)
{
    return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline vec3 operator*(const vec3 &u, const vec3 &v)
{
    return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline vec3 operator*(double t, const vec3 &u)
{
    return vec3(u.e[0] * t, u.e[1] * t, u.e[2] * t);
}

inline vec3 operator*(const vec3 &u, double t)
{
    return t * u;
}

inline vec3 operator/(const vec3 &u, double t)
{
    return (1 / t) * u;
}

inline double dot(const vec3 &u, const vec3 &v)
{
    return u.e[0] * v.e[0] + u.e[1] * v.e[1] + u.e[2] * v.e[2];
}

inline vec3 cross(const vec3 &u, const vec3 &v)
{
    return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
                u.e[2] * v.e[0] - u.e[0] * v.e[2],
                u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

inline vec3 unit_vector(vec3 v)
{
    return v / v.length();
}

// Diffuse Reflection.
// Rejection sampling method: Pick a random point in the unit cube,
// and check whether it lays inside the sphere
vec3 random_in_unit_sphere()
{
    while(true)
    {
        auto p = vec3::random(-1, 1);
        if (p.length_squared() >= 1) continue;
        return p;
    }
}

// True Lambertian Reflection: sample points on the unit sphere
vec3 random_unit_vector()
{
    return unit_vector(random_in_unit_sphere());
}

// Adopting Lambertian diffuse: reflect in a hemisphere
vec3 random_in_hemisphere(const vec3 &normal)
{
    vec3 in_unit_sphere = random_in_unit_sphere();
    if (dot(in_unit_sphere, normal) > 0.0)
        return in_unit_sphere;
    else
        return -in_unit_sphere;
}

// Lens for depth of field.
vec3 random_in_unit_disk()
{
    while (true)
    {
        auto p = vec3(random_double(-1, 1), random_double(-1, 1), 0);
        if (p.length_squared() >= 1) continue;
        return p;
    }
}

// Mirror Reflection
vec3 reflect(const vec3 &v, const vec3 &n)
{
    return v - 2 * dot(v, n) * n;
}

// Refraction
vec3 refract(const vec3 &uv, const vec3 &n, double etai_over_etat)
{
    auto cos_theta = fmin(dot(-uv, n), 1.0);
    vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n); // rafraction ray perpendicular to n
    vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.length_squared())) * n; // rafraction ray parallel to n
    return r_out_perp + r_out_parallel;
}

#endif