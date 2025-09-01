#ifndef IMAGENPGM_H
#define IMAGENPGM_H

#include "imagen.h"

class ImagenPGM : public Imagen
{
public:
    int **mat;
    ImagenPGM() : mat(nullptr) {}
    ~ImagenPGM();
    void leer() override;
    void escribir() override;
    void blur() override;
    void laplace() override;
    void sharpening() override;
};

#endif
