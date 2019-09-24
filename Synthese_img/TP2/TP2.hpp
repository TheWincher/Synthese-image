#ifndef _TP2_
#define _TP2_

#include "..\Portable_anymap\PPM.h"
#include <iostream>
#include <cmath>
#include <optional>
#include <vector>
#include <fstream>

#define WIDTH 600
#define HEIGHT 600
#define NB_POINT_LIGHT 100

struct Sphere {
	Vector3<float> center;
	float radius;

	Sphere(Vector3<float> c, float r) { center = c, radius = r; }
	Vector3<float> getNormal(const Vector3<float>& p) { return (p - center ) / radius; }
};

struct Ray {
	Vector3<float> origin;
	Vector3<float> direction;

	Ray(Vector3<float> o, Vector3<float> d) { origin = o, direction = d; }
};

struct Light
{
	Vector3<float> pos;
	Vector3<float> offset;
	Vector3<int> color;
	float intensity;

	Light(Vector3<float> p, Vector3<float> o, Vector3<int> c, float i) {
		pos = p;
		offset = o;
		color = c;
		intensity = i;
	}
};

std::optional<float> intersectionRayonSphere(const Sphere& s, const Ray& r) {

	Vector3<float> rDirectionNorm = r.direction.normalized();
	float a = 1;
	float b = 2 * (r.origin.dot(rDirectionNorm) - s.center.dot(rDirectionNorm));
	float c = r.origin.dot(r.origin) + s.center.dot(s.center) - 2 * s.center.dot(r.origin) - s.radius * s.radius;

	float delta = b * b - 4 * a * c;
	float res = -1;
	if (delta >= 0) {
		res = (-b - sqrt(delta)) / (2 * a);
		if (res < 0) {
			res = (-b + sqrt(delta)) / (2 * a);
		}
	}
	if (res < 0)
		return std::nullopt;
	return res;
}

std::optional<float> getNearestIntersect(std::vector<Sphere> spheres, const Ray& r, Sphere *s) {
	std::optional<float> res = std::nullopt;

	for (std::vector<Sphere>::iterator it2 = spheres.begin(); it2 != spheres.end(); it2++) {
		if (res.has_value()) {
			std::optional<float> tt = intersectionRayonSphere(*it2, r);
			if (tt.has_value() && tt.value() < res.value()) {
				res = tt;
				if (s != nullptr)  *s = *it2;	
			}
		}
		else {
			res = intersectionRayonSphere(*it2, r);
			if (res.has_value() && s != nullptr) {
				*s = *it2;
			}
		}
	}
	return res;
}

float clamp(float min, float max, float value)
{
	if (value < min) return min;
	if (value > max) return max;
	return value;
}
#endif
