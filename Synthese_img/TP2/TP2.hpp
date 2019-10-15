#ifndef _TP2_
#define _TP2_
#define _USE_MATH_DEFINES

#include <cmath>
#include "..\Portable_anymap\PPM.h"
#include <iostream>
#include <algorithm>
#include <functional>
#include <array>

#include <optional>
#include <vector>
#include <fstream>
#include <variant>

#define WIDTH 600
#define HEIGHT 600
#define NB_POINT_LIGHT 100
#define MAX_REBOND 3
#define NB_POINT_REFLECTED 5

struct Sphere {
	Vector3<float> center;
	Vector3<float> mat;
	float radius;
	float albedo;

	Sphere(Vector3<float> c, float r, Vector3<float> m = Vector3(1.0F,1.0F,1.0F) ,float a = 0.0f) : center(c), radius(r), mat(m), albedo(a) {}
	Vector3<float> getNormal(const Vector3<float>& p) { return (p - center ) / radius; }
};

struct Box {
	Vector3<float> lb;
	Vector3<float> rt;

	Box(Vector3<float> p1, Vector3<float> p2) { this->lb = p1, this->rt = p2; }

	Box() {}
};

struct Node;

struct Node {
	Box box;
	std::variant<Node*, Sphere> right;
	std::variant<Node*, Sphere> left;

	Node(Box b, std::variant<Node*, Sphere> r, std::variant<Node*, Sphere> l) : box(b), right(r), left(l) {}

};

struct Ray {
	Vector3<float> origin;
	Vector3<float> direction;

	Ray(Vector3<float> o, Vector3<float> d) : origin(o), direction(d) {}
};

struct Light
{
	Vector3<float> pos;
	Vector3<float> offset;
	Vector3<int> color;
	float intensity;

	Light(Vector3<float> p, Vector3<float> o, Vector3<int> c, float i) : pos(p), offset(o), color(c), intensity(i) {}
};

std::vector<Sphere> getSphereInBox(const Box& b, std::vector<Sphere> spheres) {
	std::vector<Sphere> sphereInBox;
	for (std::vector<Sphere>::iterator it = spheres.begin(); it != spheres.end(); it++) {
		if (it->center.x < b.rt.x && it->center.y < b.rt.y && it->center.z < b.rt.z)
			if (it->center.x > b.lb.x&& it->center.y > b.lb.y&& it->center.z > b.lb.z)
				sphereInBox.push_back(*it);
	}
}

Box getBox(std::vector<Sphere> spheres, int iMin, int iMax) {
	float yMin = spheres[0].center.y - spheres[0].radius;
	float yMax = spheres[0].center.y + spheres[0].radius;
	float xMin = spheres[0].center.x - spheres[0].radius;
	float xMax = spheres[0].center.x + spheres[0].radius;
	float zMin = spheres[0].center.z - spheres[0].radius;
	float zMax = spheres[0].center.z + spheres[0].radius;

	for (int i = iMin; i < iMax; i++) {
		if (spheres[i].center.y - spheres[i].radius < yMin)
			yMin = spheres[i].center.y - spheres[i].radius;
		if (spheres[i].center.y + spheres[i].radius > yMax)
			yMax = spheres[i].center.y + spheres[i].radius;
		if (spheres[i].center.x - spheres[i].radius < xMin)
			xMin = spheres[i].center.y - spheres[i].radius;
		if (spheres[i].center.x + spheres[i].radius > xMax)
			xMax = spheres[i].center.x + spheres[i].radius;
		if (spheres[i].center.z - spheres[i].radius < zMin)
			zMin = spheres[i].center.z - spheres[i].radius;
		if (spheres[i].center.z + spheres[i].radius > zMax)
			zMax = spheres[i].center.z + spheres[i].radius;
	}

	return Box(Vector3<float>(xMin, yMin, zMin), Vector3<float>(xMax, yMax, zMax));
}

void getMagnitudeAxis(std::vector<Sphere> spheres, float* x, float* y, float* z) {
	float yMin = spheres[0].center.y - spheres[0].radius;
	float yMax = spheres[0].center.y + spheres[0].radius;
	float xMin = spheres[0].center.x - spheres[0].radius;
	float xMax = spheres[0].center.x + spheres[0].radius;
	float zMin = spheres[0].center.z - spheres[0].radius;
	float zMax = spheres[0].center.z + spheres[0].radius;

	for (std::vector<Sphere>::iterator it = spheres.begin(); it != spheres.end(); it++) {
		if (it->center.y - it->radius < yMin)
			yMin = it->center.y - it->radius;
		if (it->center.y + it->radius > yMax)
			yMax = it->center.y + it->radius;
		if (it->center.x - it->radius < xMin)
			xMin = it->center.y - it->radius;
		if (it->center.x + it->radius > xMax)
			xMax = it->center.x + it->radius;
		if (it->center.z - it->radius < zMin)
			zMin = it->center.z - it->radius;
		if (it->center.z + it->radius > zMax)
			zMax = it->center.z + it->radius;
	}

	*x = xMax - xMin;
	*y = yMax - yMin;
	*z = zMax - zMin;
}

struct sortSphereX
{
	inline bool operator() (const Sphere& struct1, const Sphere& struct2)
	{
		return (struct1.center.x < struct2.center.x);
	}
};

struct sortSphereY
{
	inline bool operator() (const Sphere& struct1, const Sphere& struct2)
	{
		return (struct1.center.y < struct2.center.y);
	}
};

struct sortSphereZ
{
	inline bool operator() (const Sphere& struct1, const Sphere& struct2)
	{
		return (struct1.center.z < struct2.center.z);
	}
};

std::variant<Node*,Sphere> splitBox(const std::vector<Sphere> &spheres) {
	std::vector<Sphere> sphereInBox(spheres);//getSphereInBox(node.box, spheres);

	if (sphereInBox.size() > 1) {
		Box currentBox = getBox(sphereInBox, 0, (int) sphereInBox.size());
		float xMagnitude, yMagnitude, zMagnitude;
		getMagnitudeAxis(sphereInBox, &xMagnitude, &yMagnitude, &zMagnitude);
		if (xMagnitude >= yMagnitude && xMagnitude >= zMagnitude)
			std::sort(sphereInBox.begin(), sphereInBox.end(), sortSphereX());
		else if (yMagnitude >= xMagnitude && yMagnitude >= zMagnitude)
			std::sort(sphereInBox.begin(), sphereInBox.end(), sortSphereY());
		else
			std::sort(sphereInBox.begin(), sphereInBox.end(), sortSphereZ());

		int half = ((int) sphereInBox.size() / 2);
		std::vector<Sphere> spheresLeft, spheresRight;

		for (int i = 0; i < half; i++)
			spheresLeft.push_back(sphereInBox[i]);

		for(int i = half; i < sphereInBox.size(); i++)
			spheresRight.push_back(sphereInBox[i]);

		return new Node(currentBox, splitBox(spheresLeft), splitBox(spheresRight));
	}
	else
		return sphereInBox[0];
	
}

float clamp(float min, float max, float value)
{
	if (value < min) return min;
	if (value > max) return max;
	return value;
}

float clampMin(float min, float value)
{
	if (value < min) return min;
	return value;
}

std::optional<float> intersect(const Sphere& s, const Ray& r) {
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

	//std::cout << "res = " << res << std::endl;
	return res;
}

std::optional<float> intersect(const Box& box, const Ray& r) {
	// r.dir is unit direction vector of ray
	Vector3<float> dirfrac;
	dirfrac.x = 1.0f / r.direction.x;
	dirfrac.y = 1.0f / r.direction.y;
	dirfrac.z = 1.0f / r.direction.z;
	// lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
	// r.org is origin of ray
	float t1 = (box.lb.x - r.origin.x) * dirfrac.x;
	float t2 = (box.rt.x - r.origin.x) * dirfrac.x;
	float t3 = (box.lb.y - r.origin.y) * dirfrac.y;
	float t4 = (box.rt.y - r.origin.y) * dirfrac.y;
	float t5 = (box.lb.z - r.origin.z) * dirfrac.z;
	float t6 = (box.rt.z - r.origin.z) * dirfrac.z;

	float tmin = std::fmax(std::fmax(std::fmin(t1, t2), std::fmin(t3, t4)), std::fmin(t5, t6));
	float tmax = std::fmin(std::fmin(std::fmax(t1, t2), std::fmax(t3, t4)), std::fmax(t5, t6));

	// if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us
	// if tmin > tmax, ray doesn't intersect AABB
	if (tmax < 0 || tmin > tmax)
	{
		return std::nullopt;
	}

	return tmin;
}

std::optional<float> intersect(std::variant<Node*, Sphere> var, const Ray& r) {
	std::optional<float> res = std::nullopt;
	std::add_pointer_t<Node*> node = std::get_if<Node*>(&var);
	std::add_pointer_t<Sphere> sphere  = std::get_if<Sphere>(&var);

	if (node != nullptr)
		res = intersect((*node)->box, r);
	else if (sphere != nullptr)
		res = intersect((*sphere), r);
	else
		std::cout << "ce n'est pas possible" << "\n";

	return res;
}

std::optional<std::tuple<float, std::variant<Node*, Sphere>>> getNearestIntersect(std::variant<Node*, Sphere> var, const Ray& r) {
	//std::optional<std::tuple<float, std::variant<Node*, Sphere>>> res = std::nullopt;
	std::add_pointer_t<Node *> node;
	std::add_pointer_t<Sphere> sphere;

	if ((node = std::get_if<Node*>(&var)) != nullptr) {
		std::optional<float> tBox = intersect(var, r);
		if (tBox.has_value()) {
			std::optional<std::tuple<float, std::variant<Node*, Sphere>>> t1 = getNearestIntersect((*node)->left, r);
			std::optional<std::tuple<float, std::variant<Node*, Sphere>>> t2 = getNearestIntersect((*node)->right, r);

			//std::cout << "tBox = " << tBox.has_value() << std::endl;

			if (t1.has_value() && t2.has_value()) {
				if (std::get<0>(t1.value()) < std::get<0>(t2.value()))
					return t1;
				else
					return t2;
			}
			else if (t1.has_value())
				return t1;
			else if (t2.has_value())
				return t2;
		}
	}
	else if ((sphere = std::get_if<Sphere>(&var)) != nullptr) {
		std::optional<float> t = intersect(var, r);
		//std::cout << "t  = " << t.has_value() << std::endl;
		if (t.has_value())
			return std::make_tuple(t.value(), *sphere);
	}
	return std::nullopt;
}

//Old version
/*std::optional<std::tuple<float,const Sphere*>> getNearestIntersect(std::vector<Sphere> spheres, const Ray& r) {
	std::optional<std::tuple<float, const Sphere*>> res = std::nullopt;

	for (std::vector<Sphere>::iterator it2 = spheres.begin(); it2 != spheres.end(); it2++) {
		if (res.has_value()) {
			std::optional<float> tt = intersect(*it2, r);
			if (tt.has_value() && tt.value() < std::get<0>(res.value()))
				res = std::make_tuple(tt.value(), &*it2);
		}
		else {
			std::optional<float> tt = intersect(*it2, r);
			if (tt.has_value()) {
				res = std::make_tuple(tt.value(), &*it2);
			}
		}
	}
	return res;
}*/

Ray getReflectedRay(Ray rOrigine, Vector3<float> N, Vector3<float> pointIntersection) {
	float c1 = -N.dot(rOrigine.direction);
	Vector3<float> R = rOrigine.direction + (N * 2.0F * c1);
	return Ray(pointIntersection + R.normalized() * 0.01F, R.normalized());
}

/*Vector3<float> traceRay(Ray r, const std::vector<Sphere> &spheres, const  std::vector<Light> &lights, int nbRebon) {

	if (nbRebon >= MAX_REBOND)
		return Vector3<float>(0, 0, 0);

	Sphere** s = nullptr;
	Vector3<float> color(0, 0, 0);

	std::optional<std::tuple<float, const Sphere*>> t = getNearestIntersect(spheres, r);


	// Cas où il y a une intersection
	if (t.has_value()) {
	
		//On calcul le point d'intersection
		Vector3<float> pointIntersection = r.origin + r.direction * std::get<0>(t.value());
		Vector3<float> N = ((Sphere *)std::get<1>(t.value()))->getNormal(pointIntersection);

		if (std::get<1>(t.value())->albedo == 1.0f)
			return traceRay(getReflectedRay(r, N, pointIntersection), spheres, lights, nbRebon + 1);
		else if(std::get<1>(t.value())->albedo == -1.0f)
			return traceRay(getReflectedRay(r, N, pointIntersection), spheres, lights, nbRebon + 1);
		else {
			//Direct Light
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
						float dt = clampMin(0,Lnormalized.dot(N.normalized()));
						float dist = L.norm();

						color = color + Vector3<float>((float)lights[l].color.x, (float)lights[l].color.y, (float)lights[l].color.z) * dt * std::get<1>(t.value())->mat * lights[l].intensity / (NB_POINT_LIGHT * dist * dist);
					}
				}
			}

			//Indirect Light
			/*Vector3<float> indirectLight = Vector3<float>(0.0f, 0.0f, 0.0f);
			for (int i = 0; i < NB_POINT_REFLECTED; i++) {
				float theta = (((float)rand()) / RAND_MAX) * M_PI;
				float phi = (((float)rand()) / RAND_MAX) * M_PI;
				Vector3<float> randDirection = Vector3<float>(sin(phi) * cos(theta), sin(phi) * sin(theta), cos(phi));

				// step 3: cast the ray into the scene
				Ray randomRay(pointIntersection + randDirection.normalized() * 0.01F, randDirection.normalized());
				Vector3<float> sampleColor = traceRay(randomRay, spheres, lights, nbRebon + 1); // trace a ray from P in random direction
				// step 4 and 5: treat the return color as if it was a light (we assume our shaded surface is diffuse)
				indirectLight = indirectLight + sampleColor * cos(theta); // diffuse shading = L_i * cos(N.L) 
			}
			color = color + (indirectLight / NB_POINT_REFLECTED) * ((Sphere*)std::get<1>(t.value()))->albedo;
		}
	}
	return color;
}*/

Vector3<float> traceRay(Ray r, std::variant<Node*, Sphere> node, const  std::vector<Light>& lights, int nbRebon) {

	if (nbRebon >= MAX_REBOND)
		return Vector3<float>(0, 0, 0);

	Sphere** s = nullptr;
	Vector3<float> color(0, 0, 0);

	std::optional<std::tuple<float, std::variant<Node*, Sphere>>> t = getNearestIntersect(node, r);

	// Cas où il y a une intersection
	if (t.has_value()) {

		//On calcul le point d'intersection
		Vector3<float> pointIntersection = r.origin + r.direction * std::get<0>(t.value());
		Vector3<float> N = (std::get<Sphere>(std::get<1>(t.value()))).getNormal(pointIntersection);

		if (std::get<Sphere>(std::get<1>(t.value())).albedo == 1.0f)
			return traceRay(getReflectedRay(r, N, pointIntersection), node, lights, nbRebon + 1);
		else if (std::get<Sphere>(std::get<1>(t.value())).albedo == -1.0f)
			return traceRay(getReflectedRay(r, N, pointIntersection), node, lights, nbRebon + 1);
		else {
			//Direct Light
			for (int l = 0; l < lights.size(); l++) {
				for (int i = 0; i < NB_POINT_LIGHT; i++) {
					float xRand = (((float)rand()) / RAND_MAX) * lights[l].offset.x;
					float yRand = (((float)rand()) / RAND_MAX) * lights[l].offset.y;
					float zRand = (((float)rand()) / RAND_MAX) * lights[l].offset.z;

					Vector3<float> vRand(xRand, yRand, zRand);
					Vector3<float> L = (lights[l].pos + vRand) - pointIntersection;
					Vector3<float> Lnormalized = L.normalized();

					Ray r2 = { pointIntersection + Lnormalized * 0.01F, Lnormalized };
					std::optional<std::tuple<float, std::variant<Node*, Sphere>>> t2 = getNearestIntersect(node, r2);

					if (t2.has_value() && (Lnormalized * std::get<0>(t2.value())).norm() < L.norm())
						color = color + Vector3<float>(0, 0, 0);
					else {
						float dt = clampMin(0, Lnormalized.dot(N.normalized()));
						float dist = L.norm();

						color = color + Vector3<float>((float)lights[l].color.x, (float)lights[l].color.y, (float)lights[l].color.z) * dt * std::get<Sphere>(std::get<1>(t.value())).mat * lights[l].intensity / (NB_POINT_LIGHT * dist * dist);
					}
				}
			}

			//Indirect Light
			/*Vector3<float> indirectLight = Vector3<float>(0.0f, 0.0f, 0.0f);
			for (int i = 0; i < NB_POINT_REFLECTED; i++) {
				float theta = (((float)rand()) / RAND_MAX) * M_PI;
				float phi = (((float)rand()) / RAND_MAX) * M_PI;
				Vector3<float> randDirection = Vector3<float>(sin(phi) * cos(theta), sin(phi) * sin(theta), cos(phi));

				// step 3: cast the ray into the scene
				Ray randomRay(pointIntersection + randDirection.normalized() * 0.01F, randDirection.normalized());
				Vector3<float> sampleColor = traceRay(randomRay, spheres, lights, nbRebon + 1); // trace a ray from P in random direction
				// step 4 and 5: treat the return color as if it was a light (we assume our shaded surface is diffuse)
				indirectLight = indirectLight + sampleColor * cos(theta); // diffuse shading = L_i * cos(N.L)
			}
			color = color + (indirectLight / NB_POINT_REFLECTED) * ((Sphere*)std::get<1>(t.value()))->albedo;*/
		}
	}
	return color;
}
#endif
