#ifndef IMAGENPPM_H
#define IMAGENPPM_H

#include "imagen.h"

struct Pixel
{
    int r, g, b;
};

class ImagenPPM : public Imagen
{
public:
    Pixel **mat;
    ImagenPPM() : mat(nullptr) {}
    ~ImagenPPM();
    void leer() override;
    void escribir() override;
    void blur() override;
    void laplace() override;
    void sharpening() override;
};

#endif
