#ifndef ONBH
#define ONBH

#include "vec3.h"


class onb {
public:
	onb() {}

	inline vector3 operator[](int i) const { return axis[i]; }

	vector3 u() const { return axis[0]; }
	vector3 v() const { return axis[1]; }
	vector3 w() const { return axis[2]; }

	vector3 local(double a, double b, double c) const {
		return a * u() + b * v() + c * w();
	}

	vector3 local(const vector3& a) const {
		return a.x() * u() + a.y() * v() + a.z() * w();
	}

	void build_from_w(const vector3&);

public:
	vector3 axis[3];
};


void onb::build_from_w(const vector3& n) {
	axis[2] = unit_vector(n);
	vector3 a = (fabs(w().x()) > 0.9) ? vector3(0, 1, 0) : vector3(1, 0, 0);
	axis[1] = unit_vector(cross(w(), a));
	axis[0] = cross(w(), v());
}

#endif