#include <fstream>
#include "PGM.h"

PGM::PGM(const int &width, const int &height, const int &maxValue) : PortableAnymap(width, height, maxValue) {
}

PGM PGM::load(const std::string &path) {
    std::ifstream file(path);
    std::string magicNumber;
    file >> magicNumber;
    int width, height, maxValue;
    file >> width;
    file >> height;
    file >> maxValue;
    PGM image(width, height, maxValue);

    int i = 0;
    int val;
    while (file >> image.pixelMatrix[i / width][i % width]) {
        ++i;
    }
    file.close();
    return image;
}

std::ostream &operator<<(std::ostream &os, const PGM &pgm) {
    os << "P2" << std::endl;
    os << pgm.width << " " << pgm.height << std::endl;
    os << pgm.maxValue << std::endl;
    for (int row = 0; row < pgm.height; ++row) {
        os << pgm.pixelMatrix[row][0];
        for (int col = 1; col < pgm.width; ++col) {
            os << " " << pgm.pixelMatrix[row][col];
        }
        os << std::endl;
    }
    return os;
}

void PGM::save(const std::string &path) const {
    std::ofstream file(path);
    file << *this;
    file.close();
}
