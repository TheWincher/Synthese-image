#include "TP2.hpp"
#include <time.h>
#include <chrono>

std::vector<Sphere> generateSpheres(int nbSphere) {
	int cpt = 0;
	float x = 50, y = 50, z = 100;
	std::vector<Sphere> spheres;
	while (cpt < nbSphere) {
		spheres.push_back(Sphere(Vector3<float>(x, y, z), 40.0f));
		cpt++;
		x += 90;
		if (x >= WIDTH) {
			y += 90;
			x = 50;
		}

		if (y > HEIGHT) {
			z += 90;
			y = 50;
		}
	}
	return spheres;
}

int main() {

	Vector3<float> pointCamera = Vector3<float>(300.f, 300.f, -450.f);
	unsigned long nbRayonLance = 0;

	//Création d'un tableau de spheres
	auto start = std::chrono::system_clock::now();
	std::vector<Sphere> spheres = generateSpheres(1000);
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> timeToGenerate = end - start;
	std::cout << "Temps pour générer les sphères = " << timeToGenerate.count() << std::endl;
	srand((unsigned int)time(NULL));

	//On ajoute des spheres dans le tableau
	//spheres.push_back(Sphere(Vector3<float>(300,300,300),50));
	/*spheres.push_back(Sphere(Vector3<float>(400,400,100), 50, Vector3<float>(1.0F,1.0F,1.0F), 1.0f));

	float R = 15000.0;
	spheres.push_back(Sphere(Vector3<float>(300.f, 300.f, R + 500.f), R, Vector3<float>(1.f, 0.f, 0.f), 0.5f));//fond
	spheres.push_back(Sphere(Vector3<float>{300.f, 300.f, -R + 00.f}, R, Vector3<float>(1.f, 0.f, 0.f)));//devant
	spheres.push_back(Sphere(Vector3<float>{300.f, R + 600.f, 0.f}, R,  Vector3<float>(0.f, 1.f, 0.f)));//droite
	spheres.push_back(Sphere(Vector3<float>{300.f, -R, 0.f}, R,  Vector3<float>(1.f, 0.f, 1.f)));//gauche
	spheres.push_back(Sphere(Vector3<float>{R + 600.f, 300.f, 0.f}, R, Vector3<float>(0.f, 1.f, 1.f)));//bas
	spheres.push_back(Sphere(Vector3<float>{-R, 300.f, 0.f}, R,Vector3<float>(1.f, 1.f, 0.f)));//haut  */

	//Création du rayon et de la lumière
	std::vector<Light> lights;
	lights.push_back(Light(Vector3<float>(300.f, 300.f, 100.f), Vector3<float>(20, 20, 10), Vector3<int>(255, 255, 255), 100000));
	//lights.push_back(Light(Vector3<float>(100, 500, 150), Vector3<float>(20, 20, 10), Vector3<int>(255, 255, 0), 200000));

	//Création d'une image 
	PPM ppm(WIDTH, HEIGHT, 255);

	std::variant<Node*, Sphere> root = splitBox(spheres);

	//Pour chaque pixel
#pragma omp parallel for
	auto start_ray = std::chrono::system_clock::now();
	for (int y = 0; y < HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {

			Vector3<float> direction = Vector3<float>((float) x, (float) y, 0) - pointCamera;
			//On envoit un rayon
			Ray r(Vector3<float>((float) x, (float) y,0),direction.normalized());
			//Vector3<float> color = traceRay(r, root, lights, 0, &nbRayonLance);
			Vector3<float> color = traceRay(r, spheres, lights, 0, &nbRayonLance);
			ppm.pixelMatrix[y][x] = Vector3<int>((int)clamp(0, 255, color.x), (int)clamp(0, 255, color.y), (int)clamp(0, 255, color.z));
		}
	}
	auto end_ray = std::chrono::system_clock::now();
	std::chrono::duration<double> timeRayTracer = end_ray - start_ray;
	std::cout << "Temps passé dans le ray tracer = " << timeRayTracer.count() << std::endl;
	std::cout << "Nombre de rayon lancé : " << nbRayonLance << std::endl;

	ppm.save("image.ppm");
	return 0;
}

