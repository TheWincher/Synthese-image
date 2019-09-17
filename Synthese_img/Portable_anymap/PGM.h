#ifndef IMAGE_SYNTHESIS_PGM_H
#define IMAGE_SYNTHESIS_PGM_H

#include <ostream>
#include "PortableAnymap.h"

class PGM : public PortableAnymap<int> {
public:
    PGM(const int &width, const int &height, const int &maxValue);

    void save(const std::string &path) const override;

    static PGM load(const std::string &path);

    friend std::ostream &operator<<(std::ostream &os, const PGM &pgm);
};

#endif //IMAGE_SYNTHESIS_PGM_H
