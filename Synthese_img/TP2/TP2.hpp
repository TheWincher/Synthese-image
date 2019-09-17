#ifndef _TP2_
#define _TP2_

#include "..\Portable_anymap\PPM.h"
#include <iostream>
#include <cmath>
#include <optional>

#define WIDTH 600
#define HEIGHT 600

struct sphere {
	Vector3<float> centre;
	float rayon;
};

struct rayon {
	Vector3<float> origine;
	Vector3<float> direction;
};

struct light
{
	Vector3<float> position;
	Vector3<float> intensite;
};

std::optional<float> intersectionRayonSphere(const sphere& s, const rayon& r) {

	Vector3<float> rDirectionNorm = r.direction.normalized();
	float a = 1;
	float b = 2 * (r.origine.dot(rDirectionNorm) - s.centre.dot(rDirectionNorm));
	float c = r.origine.dot(r.origine) + s.centre.dot(s.centre) - 2 * s.centre.dot(r.origine) - s.rayon * s.rayon;

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
#endif
