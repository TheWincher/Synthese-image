#include "TP2.hpp"
#include <time.h>

#pragma exp parallel for
int main() {

	//Création d'un tableau de spheres
	std::vector<Sphere> spheres;
	srand(time(NULL));

	//On ajoute des spheres dans le tableau
	spheres.push_back(Sphere(Vector3<float>(300,300,300),50));
	spheres.push_back(Sphere(Vector3<float>(600,600,600),250));

	//Création du rayon et de la lumière
	std::vector<Light> lights;
	lights.push_back(Light(Vector3<float>(100, 100, 0), Vector3<float>(20, 20, 10), Vector3<int>(0, 0, 255), 300000));
	lights.push_back(Light(Vector3<float>(100, 500, 0), Vector3<float>(20, 20, 10), Vector3<int>(255, 255, 0), 200000));

	//Création d'une image 
	PPM ppm(WIDTH, HEIGHT, 255);

	//Pour chaque pixel
	for (int y = 0; y < HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {

			//On envoit un rayon
			Ray r = { Vector3<float>(x,y,0),Vector3<float>(0,0,1) };

			Sphere *s = (Sphere *) malloc(sizeof(struct Sphere));
			std::optional<float> t = getNearestIntersect(spheres, r, s);

			// Cas où il y a une intersection
			if (t.has_value()) {

				//On calcul le point d'intersection
				Vector3<float> pointIntersection = r.origin + r.direction * t.value();

				//
				Vector3<float> N = s->getNormal(pointIntersection);
				Vector3<float> color(0,0,0);

				for (int l = 0; l < lights.size(); l++) {
					for (int i = 0; i < NB_POINT_LIGHT; i++) {
						float xRand = rand() % ((int)lights[l].offset.x) + 1;
						float yRand = rand() % ((int)lights[l].offset.y) + 1;
						float zRand = rand() % ((int)lights[l].offset.z) + 1;

						Vector3<float> vRand(xRand,yRand, zRand);
						Vector3<float> L = (lights[l].pos + vRand) - pointIntersection;
						Vector3<float> Lnormalized = L.normalized();

						Ray r2 = { pointIntersection + Lnormalized * 0.01F, Lnormalized };
						std::optional<float> t2 = getNearestIntersect(spheres, r2, nullptr);

						if (t2.has_value() && (Lnormalized * t2.value()).norm() < L.norm())
							color = color + Vector3<float>(0, 0, 0);
						else {
							float dt = Lnormalized.dot(N.normalized());
							float dist = L.norm();
							if (dt < 0) dt = 0;

							color = color + Vector3<float>(lights[l].color.x * dt, lights[l].color.y * dt, lights[l].color.z * dt) * lights[l].intensity / (NB_POINT_LIGHT * dist * dist);
						}
					}
				}
				ppm.pixelMatrix[y][x] = Vector3<int>((int)clamp(0,255,color.x), (int)clamp(0,255,color.y), (int)clamp(0,255,color.z));
			}
		}
	}

	ppm.save("image.ppm");
	return 0;
}

