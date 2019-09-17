#include <fstream>
#include "PPM.h"

PPM::PPM(const int &width, const int &height, const int &maxValue) : PortableAnymap(width, height, maxValue) {
}

PPM PPM::load(const std::string &path) {
    std::ifstream file(path);
    std::string magicNumber;
    file >> magicNumber;
    int width, height, maxValue;
    file >> width;
    file >> height;
    file >> maxValue;
    PPM image(width, height, maxValue);

    int i = 0;
    int r, g, b;
    while (file >> r >> g >> b) {
        image.pixelMatrix[i / width][i % width] = Vector3<int>(r, g, b);
        ++i;
    }
    file.close();
    return image;
}

std::ostream &operator<<(std::ostream &os, const PPM &pgm) {
    os << "P3" << std::endl;
    os << pgm.width << " " << pgm.height << std::endl;
    os << pgm.maxValue << std::endl;
    for (int row = 0; row < pgm.height; ++row) {
        const Vector3<int> &firstPixel = pgm.pixelMatrix[row][0];
        os << firstPixel.x << " " << firstPixel.y << " " << firstPixel.z;
        for (int col = 1; col < pgm.width; ++col) {
            const Vector3<int> &pixel = pgm.pixelMatrix[row][col];
            os << " " << pixel.x << " " << pixel.y << " " << pixel.z;
        }
        os << std::endl;
    }
    return os;
}

void PPM::save(const std::string &path) const {
    std::ofstream file(path);
    file << *this;
    file.close();
}
