#ifndef IMAGEN_H
#define IMAGEN_H

#include <cstdio>

class Imagen
{
public:
    int N;     // ancho
    int M;     // alto
    int mxVal; // valor máximo

    virtual ~Imagen() {}

    virtual void leer() = 0;
    virtual void escribir(FILE *out) = 0;

    virtual void blur() = 0;
    virtual void laplace() = 0;
    virtual void sharpening() = 0;

    // clonar la imagen para filtros en paralelo
    virtual Imagen *clonar() = 0;
};

#endif
