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
#define MAX_REBOND 5

struct Sphere {
	Vector3<float> center;
	Vector3<float> mat;
	float radius;
	bool isReflective;

	Sphere(Vector3<float> c, float r, Vector3<float> m = Vector3(1.0F,1.0F,1.0F) ,bool b = false) { center = c, radius = r, mat = m, isReflective = b; }
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

std::optional<std::tuple<float,const Sphere*>> getNearestIntersect(std::vector<Sphere> spheres, const Ray& r) {
	std::optional<std::tuple<float, const Sphere*>> res = std::nullopt;

	for (std::vector<Sphere>::iterator it2 = spheres.begin(); it2 != spheres.end(); it2++) {
		if (res.has_value()) {
			std::optional<float> tt = intersectionRayonSphere(*it2, r);
			if (tt.has_value() && tt.value() < std::get<0>(res.value()))
				res = std::make_tuple(tt.value(), &*it2);
		}
		else {
			std::optional<float> tt = intersectionRayonSphere(*it2, r);
			if (tt.has_value()) {
				res = std::make_tuple(tt.value(), &*it2);
			}
		}
	}
	return res;
}

Ray getReflectedRay(Ray rOrigine, Vector3<float> N, Vector3<float> pointIntersection) {
	float c1 = -N.dot(rOrigine.direction);
	Vector3<float> R = rOrigine.direction + (N * 2.0F * c1);
	return Ray(pointIntersection + R.normalized() * 0.01F, R.normalized());
}

Vector3<float> traceRay(Ray r, const std::vector<Sphere> &spheres, const  std::vector<Light> &lights) {

	Sphere** s = nullptr;
	Vector3<float> color(0, 0, 0);

	std::optional<std::tuple<float, const Sphere*>> t = getNearestIntersect(spheres, r);


	// Cas où il y a une intersection
	if (t.has_value()) {
	
		//On calcul le point d'intersection
		Vector3<float> pointIntersection = r.origin + r.direction * std::get<0>(t.value());
		Vector3<float> N = ((Sphere *)std::get<1>(t.value()))->getNormal(pointIntersection);

		if (std::get<1>(t.value())->isReflective) {
			if (std::get<1>(t.value())->isReflective)
				return traceRay(getReflectedRay(r, N, pointIntersection), spheres, lights);
		}

		for (int l = 0; l < lights.size(); l++) {
			for (int i = 0; i < NB_POINT_LIGHT; i++) {
				float xRand = (((float)rand()) / RAND_MAX) * lights[l].offset.x;
				float yRand = (((float)rand()) / RAND_MAX) * lights[l].offset.y;
				float zRand = (((float)rand()) / RAND_MAX) * lights[l].offset.z;

				Vector3<float> vRand(xRand, yRand, zRand);
				Vector3<float> L = (lights[l].pos + vRand) - pointIntersection;
				Vector3<float> Lnormalized = L.normalized();

				Ray r2 = { pointIntersection + Lnormalized * 0.01F, Lnormalized };
				std::optional<std::tuple<float, const Sphere*>> t2 = getNearestIntersect(spheres, r2);

				if (t2.has_value() && (Lnormalized * std::get<0>(t2.value())).norm() < L.norm())
					color = color + Vector3<float>(0, 0, 0);
				else {
					float dt = Lnormalized.dot(N.normalized());
					float dist = L.norm();
					if (dt < 0) dt = 0;

					Vector3<float> colorReflected = Vector3<float>(0.0F, 0.0F, 0.0F);
					if (std::get<1>(t.value())->isReflective) {
						colorReflected = traceRay(getReflectedRay(r, N, pointIntersection), spheres, lights);
						return colorReflected;
					}
					color = color + Vector3<float>((float) lights[l].color.x, (float) lights[l].color.y, (float) lights[l].color.z) * dt * std::get<1>(t.value())->mat * lights[l].intensity / (NB_POINT_LIGHT * dist * dist);
				}
			}
		}
	}
	return color;
}

float clamp(float min, float max, float value)
{
	if (value < min) return min;
	if (value > max) return max;
	return value;
}
#endif
