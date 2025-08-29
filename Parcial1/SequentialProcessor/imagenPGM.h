#ifndef IMAGENPGM_H
#define IMAGENPGM_H

#include "Imagen.h"

class ImagenPGM : public Imagen
{
    int **mat;

public:
    ImagenPGM() : mat(nullptr) {}
    ~ImagenPGM();
    void leer() override;
    void escribir() override;
    void blur() override;
    void laplace() override;
    void sharpening() override;
};

#endif
