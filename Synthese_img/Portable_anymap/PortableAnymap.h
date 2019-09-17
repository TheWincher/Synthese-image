#ifndef IMAGE_SYNTHESIS_PORTABLEANYMAP_H
#define IMAGE_SYNTHESIS_PORTABLEANYMAP_H

#include <string>
#include <ostream>

template<typename T>
class PortableAnymap {
protected:
    PortableAnymap(const int &width, const int &height, const int &maxValue) {
        this->width = width;
        this->height = height;
        this->maxValue = maxValue;
        pixelMatrix = new T *[height];
        for (int row = 0; row < height; ++row) {
            pixelMatrix[row] = new T[width];
        }
    }

public:
    int width, height, maxValue;
    T **pixelMatrix;

    ~PortableAnymap() {
        for (int i = 0; i < height; ++i) {
            delete[] pixelMatrix[i];
        }
        delete[] pixelMatrix;
    }

    virtual void save(const std::string &path) const = 0;
};

#endif //IMAGE_SYNTHESIS_PORTABLEANYMAP_H
