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
    virtual void leer() = 0;
    virtual void procesar() = 0;
    virtual void escribir() = 0;
    virtual void blur() = 0;
    virtual ~Imagen() {}
};

#endif
