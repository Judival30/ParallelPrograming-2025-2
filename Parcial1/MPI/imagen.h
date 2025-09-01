#ifndef IMAGEN_H
#define IMAGEN_H

#include <iostream>
using namespace std;

class Imagen
{
public:
    int N, M, mxVal; // columnas, filas, valor máximo
    virtual void leer() = 0;
    virtual void escribir() = 0;
    virtual void blur() = 0;
    virtual void laplace() = 0;
    virtual void sharpening() = 0;
    virtual ~Imagen() {}
};

#endif
