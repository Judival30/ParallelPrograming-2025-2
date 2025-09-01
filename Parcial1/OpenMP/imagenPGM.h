#ifndef IMAGENPGM_H
#define IMAGENPGM_H

#include "imagen.h"
#include <iostream>
using namespace std;

class ImagenPGM : public Imagen
{
public:
    int **mat;

    ImagenPGM()
    {
        mat = nullptr;
        N = M = mxVal = 0;
    }
    ~ImagenPGM();

    void leer();
    void escribir(FILE *out);

    void blur();
    void laplace();
    void sharpening();

    Imagen *clonar();
};

#endif
