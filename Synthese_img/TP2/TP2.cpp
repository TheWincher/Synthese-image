#include "TP2.hpp"
#include "../Portable_anymap/PPM.h"
#include <vector>

int main() {

	std::vector<sphere> spheres;
	spheres.push_back(sphere{Vector3<float>(200,200,300),100});
	spheres.push_back(sphere{ Vector3<float>(400,400,300),50 });
	rayon r = { Vector3<float>(0,0,0),Vector3<float>(0,0,1) };
	light L = { Vector3<float>(300,600,300),Vector3<float>(255, 255, 255) };

	PPM ppm(WIDTH, HEIGHT, 255);
	for (int row = 0; row < HEIGHT; row++) {
		for (int col = 0; col < WIDTH; col++) {
			r.origine.x = (float) col;
			r.origine.y = (float) row;
			for (std::vector<sphere>::iterator it = spheres.begin(); it != spheres.end(); it++) {
				std::optional<float> t = intersectionRayonSphere(*it, r);
				if (t.has_value()) {
					//ppm.pixelMatrix[row][col] = Vector3<int>(255, 255, 255);
					Vector3<float> pointIntersection = r.origine + r.direction * t.value();
					rayon r2 = { pointIntersection, (L.position - pointIntersection).normalized() };
					for (std::vector<sphere>::iterator it2 = spheres.begin(); it2 != spheres.end(); it2++) {
						std::optional<float> t2 = intersectionRayonSphere(*it2, r2);
						if (t2.has_value() && (pointIntersection + (L.position - pointIntersection).normalized() * t2.value()).norm() < (L.position - pointIntersection).norm()) {
							//intersection avant la lumiere donc dans l'ombre
							ppm.pixelMatrix[row][col] = Vector3<int>(0, 0, 0);
							break;
						}
						else {
							Vector3<float> lightDirection = (L.position - pointIntersection);
							Vector3<float> normale = (pointIntersection - it->centre);

							float angle = (lightDirection.dot(normale)) / (lightDirection.norm() * normale.norm());
							float dist = lightDirection.norm();

							Vector3<float> Lres = Vector3<float>(angle, angle, angle) * L.intensite;
							ppm.pixelMatrix[row][col] = Vector3<int>(Lres.x, Lres.y, Lres.z);
							//ppm.pixelMatrix[row][col] = Vector3<int>(255, 255, 255); 
						}
					}		
				}
				else {
					ppm.pixelMatrix[row][col] = ppm.pixelMatrix[row][col] + Vector3<int>(0, 0, 0);
				}
			}		
		}
	}

	ppm.save("image.ppm");
}

// Exécuter le programme : Ctrl+F5 ou menu Déboguer > Exécuter sans débogage
// Déboguer le programme : F5 ou menu Déboguer > Démarrer le débogage

// Astuces pour bien démarrer : 
//   1. Utilisez la fenêtre Explorateur de solutions pour ajouter des fichiers et les gérer.
//   2. Utilisez la fenêtre Team Explorer pour vous connecter au contrôle de code source.
//   3. Utilisez la fenêtre Sortie pour voir la sortie de la génération et d'autres messages.
//   4. Utilisez la fenêtre Liste d'erreurs pour voir les erreurs.
//   5. Accédez à Projet > Ajouter un nouvel élément pour créer des fichiers de code, ou à Projet > Ajouter un élément existant pour ajouter des fichiers de code existants au projet.
//   6. Pour rouvrir ce projet plus tard, accédez à Fichier > Ouvrir > Projet et sélectionnez le fichier .sln.
