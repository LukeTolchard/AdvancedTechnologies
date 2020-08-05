#ifndef VEC3_H
#define VEC3_H
//==============================================================================================
// Originally written in 2016 by Peter Shirley <ptrshrl@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related and
// neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy (see file COPYING.txt) of the CC0 Public Domain Dedication
// along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
//==============================================================================================

#include <cmath>
#include <iostream>

using std::sqrt;

class vector3 {
    public:
        vector3() : e{0,0,0} {}
        vector3(double e0, double e1, double e2) : e{e0, e1, e2} {}

        double x() const { return e[0]; }
        double y() const { return e[1]; }
        double z() const { return e[2]; }

        vector3 operator-() const { return vector3(-e[0], -e[1], -e[2]); }
        double operator[](int i) const { return e[i]; }
        double& operator[](int i) { return e[i]; }

        vector3& operator+=(const vector3 &v) {
            e[0] += v.e[0];
            e[1] += v.e[1];
            e[2] += v.e[2];
            return *this;
        }

        vector3& operator*=(const double t) {
            e[0] *= t;
            e[1] *= t;
            e[2] *= t;
            return *this;
        }

        vector3& operator/=(const double t) {
            return *this *= 1/t;
        }

        double length() const {
            return sqrt(length_squared());
        }

        double length_squared() const {
            return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
        }

        void write_color(std::ostream &out, int samples_per_pixel) {
            // Replace NaN component values with zero.
            // See explanation in Ray Tracing: The Rest of Your Life.
            if (e[0] != e[0]) e[0] = 0.0;
            if (e[1] != e[1]) e[1] = 0.0;
            if (e[2] != e[2]) e[2] = 0.0;

            // Divide the color total by the number of samples and gamma-correct
            // for a gamma value of 2.0.
            auto scale = 1.0 / samples_per_pixel;
            auto r = sqrt(scale * e[0]);
            auto g = sqrt(scale * e[1]);
            auto b = sqrt(scale * e[2]);

            // Write the translated [0,255] value of each color component.
            out << static_cast<int>(256 * clamp(r, 0.0, 0.999)) << ' '
                << static_cast<int>(256 * clamp(g, 0.0, 0.999)) << ' '
                << static_cast<int>(256 * clamp(b, 0.0, 0.999)) << '\n';
        }

        inline static vector3 random() {
            return vector3(random_double(), random_double(), random_double());
        }

        inline static vector3 random(double min, double max) {
            return vector3(random_double(min,max), random_double(min,max), random_double(min,max));
        }

    public:
        double e[3];
};


// Type aliases for vec3
using point = vector3;   // 3D point
using colour = vector3;    // RGB color


// vec3 Utility Functions

inline std::ostream& operator<<(std::ostream &out, const vector3 &v) {
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline vector3 operator+(const vector3 &u, const vector3 &v) {
    return vector3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline vector3 operator-(const vector3 &u, const vector3 &v) {
    return vector3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline vector3 operator*(const vector3 &u, const vector3 &v) {
    return vector3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline vector3 operator*(double t, const vector3 &v) {
    return vector3(t*v.e[0], t*v.e[1], t*v.e[2]);
}

inline vector3 operator*(const vector3 &v, double t) {
    return t * v;
}

inline vector3 operator/(vector3 v, double t) {
    return (1/t) * v;
}

inline double dot(const vector3 &u, const vector3 &v) {
    return u.e[0] * v.e[0]
         + u.e[1] * v.e[1]
         + u.e[2] * v.e[2];
}

inline vector3 cross(const vector3 &u, const vector3 &v) {
    return vector3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
                u.e[2] * v.e[0] - u.e[0] * v.e[2],
                u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

inline vector3 unit_vector(vector3 v) {
    return v / v.length();
}

inline vector3 random_in_unit_disk() {
    while (true) {
        auto p = vector3(random_double(-1,1), random_double(-1,1), 0);
        if (p.length_squared() >= 1) continue;
        return p;
    }
}

inline vector3 random_unit_vector() {
    auto a = random_double(0, 2*pi);
    auto z = random_double(-1, 1);
    auto r = sqrt(1 - z*z);
    return vector3(r*cos(a), r*sin(a), z);
}

inline vector3 random_in_unit_sphere() {
    while (true) {
        auto p = vector3::random(-1,1);
        if (p.length_squared() >= 1) continue;
        return p;
    }
}

inline vector3 random_in_hemisphere(const vector3& normal) {
    vector3 in_unit_sphere = random_in_unit_sphere();
    if (dot(in_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
        return in_unit_sphere;
    else
        return -in_unit_sphere;
}

inline vector3 reflect(const vector3& v, const vector3& n) {
    return v - 2*dot(v,n)*n;
}

inline vector3 refract(const vector3& uv, const vector3& n, double etai_over_etat) {
    auto cos_theta = fmin(dot(-uv, n), 1.0);
    vector3 r_out_parallel =  etai_over_etat * (uv + cos_theta*n);
    vector3 r_out_perp = -sqrt(1.0 - r_out_parallel.length_squared()) * n;
    return r_out_parallel + r_out_perp;
}


inline vector3 random_cosine_direction() {
	float r1 = random_double();
	float r2 = random_double();
	float z = sqrt(1 - r2);
	float phi = 2 * pi * r1;
	float x = cos(phi) * sqrt(r2);
	float y = sin(phi) * sqrt(r2);
	return vector3(x, y, z);
}

inline vector3 random_to_sphere(double radius, double distance_squared) {
	auto r1 = random_double();
	auto r2 = random_double();
	auto z = 1 + r2 * (sqrt(1 - radius * radius / distance_squared) - 1);

	auto phi = 2 * pi * r1;
	auto x = cos(phi) * sqrt(1 - z * z);
	auto y = sin(phi) * sqrt(1 - z * z);

	return vector3(x, y, z);
}

#endif
