#ifndef IMAGENPPM_H
#define IMAGENPPM_H

#include "Imagen.h"

struct Pixel
{
    int r, g, b;
};

class ImagenPPM : public Imagen
{
    Pixel **mat;

public:
    ImagenPPM() : mat(nullptr) {}
    ~ImagenPPM();
    void leer() override;
    void escribir() override;
    void blur() override;
    void laplace() override;
    void sharpening() override;
};

#endif
