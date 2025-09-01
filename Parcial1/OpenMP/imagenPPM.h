#ifndef IMAGENPPM_H
#define IMAGENPPM_H

#include "imagen.h"
#include <iostream>
using namespace std;

struct Pixel
{
    int r, g, b;
};

class ImagenPPM : public Imagen
{
public:
    Pixel **mat;

    ImagenPPM()
    {
        mat = nullptr;
        N = M = mxVal = 0;
    }
    ~ImagenPPM();

    void leer();
    void escribir(FILE *out);

    void blur();
    void laplace();
    void sharpening();

    Imagen *clonar();
};

#endif
