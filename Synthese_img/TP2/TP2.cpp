#include "TP2.hpp"

int main() {

	//Création d'un tableau de spheres
	std::vector<Sphere> spheres;

	//On ajoute des spheres dans le tableau
	spheres.push_back(Sphere(Vector3<float>(400,400,300),100));
	spheres.push_back(Sphere(Vector3<float>(200,200,300),50 ));

	//Création du rayon et de la lumière
	Light light = { Vector3<float>(100,0,300),Vector3<int>(255, 255, 255) };

	//Création d'une image 
	PPM ppm(WIDTH, HEIGHT, 255);

	//Pour chaque pixel
	for (int y = 0; y < HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {

			//On envoit un rayon
			Ray r = { Vector3<float>(x,y,0),Vector3<float>(0,0,1) };

			//On regarde si il y a une intersection avec chaque object de la scène
			for (std::vector<Sphere>::iterator it = spheres.begin(); it != spheres.end(); it++) {
				std::optional<float> t = intersectionRayonSphere(*it, r);

				/*if (x == 390 && y == 306) {
					std::cout << "-----------------------------" << std::endl;
					std::cout << t.has_value() << std::endl;
				}*/
				// Cas où il y a une intersection
				if (t.has_value()) {

					//On calcul le point d'intersection
					Vector3<float> pointIntersection = r.origin + r.direction * t.value();

					//
					Vector3<float> L = light.pos - pointIntersection;
					Vector3<float> N = it->getNormal(pointIntersection);

					Ray r2 = { pointIntersection + L.normalized() * 0.01F, L.normalized() };
					std::optional<float> t2;

					for (std::vector<Sphere>::iterator it2 = spheres.begin(); it2 != spheres.end(); it2++) {

						if (x == 390 && y == 306) std::cout << t.has_value() << std::endl;
						if (t2.has_value()) {
							std::optional<float> tt = intersectionRayonSphere(*it2, r2);
							if (tt.has_value() && tt.value() < t2.value() && (L.normalized() * tt.value()).norm() > FLT_EPSILON)
								t2 = tt;
						}
						else {
							t2 = intersectionRayonSphere(*it2, r2);
						}
					}

					if (t2.has_value() && (L.normalized() * t2.value()).norm() < L.norm())
						ppm.pixelMatrix[y][x] = Vector3<int>(0, 0, 0);
					else {

						float dt = L.normalized().dot(N.normalized());
						float dist = L.norm();

						Vector3<int> color(light.color.x * dt, light.color.y * dt, light.color.z * dt);
						ppm.pixelMatrix[y][x] = color;
					}
				}
			}		
		}
	}

	ppm.save("image.ppm");
	return 0;
}

