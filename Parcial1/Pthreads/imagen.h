#ifndef IMAGEN_H
#define IMAGEN_H

#include <iostream>
#include <cstdlib>
using namespace std;

class Imagen
{
protected:
    int N, M, mxVal;

public:
    // número de hilos (por defecto se define en main.cpp)
    static int numThreads;
    static void setNumThreads(int t);

    virtual void leer() = 0;
    virtual void escribir() = 0;
    virtual void blur() = 0;
    virtual void laplace() = 0;
    virtual void sharpening() = 0;
    virtual ~Imagen() {}
};

#endif
