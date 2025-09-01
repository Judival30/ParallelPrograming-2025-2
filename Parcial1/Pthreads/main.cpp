#include "ImagenPGM.h"
#include "ImagenPPM.h"
#include <cstring>
#include <iostream>
#include <chrono>
using namespace std;

int Imagen::numThreads = 4;
void Imagen::setNumThreads(int t)
{
    if (t > 0)
        numThreads = t;
}

int main(int argc, char *argv[])
{

    freopen(argv[1], "r", stdin);
    freopen(argv[2], "w", stdout);

    if (argc >= 6)
    {
        int nt = atoi(argv[5]);
        if (nt > 0)
            Imagen::setNumThreads(nt);
    }

    char tipo[3];
    cin >> tipo;

    for (int i = (int)strlen(tipo) - 1; i >= 0; i--)
        cin.putback(tipo[i]);

    Imagen *img = nullptr;
    if (tipo[0] == 'P' && tipo[1] == '2')
        img = new ImagenPGM();
    else if (tipo[0] == 'P' && tipo[1] == '3')
        img = new ImagenPPM();

    img->leer();

    auto start = chrono::high_resolution_clock::now();
    char *flag = argv[3];
    char *filter = argv[4];
    if (strcmp(flag, "--f") == 0)
    {
        cerr << "Aplicando filtro: " << filter << " (hilos=" << Imagen::numThreads << ")\n";
        if (strcmp(filter, "blur") == 0)
            img->blur();
        else if (strcmp(filter, "sharpening") == 0)
            img->sharpening();
        else if (strcmp(filter, "laplace") == 0)
            img->laplace();
        else
            cerr << "Filtro no reconocido: " << filter << "\n";
    }
    else if (strcmp(flag, "--all") == 0)
    {
        cerr << "Aplicando todos los filtros... (hilos=" << Imagen::numThreads << ")\n";
        img->blur();
        img->laplace();
        img->sharpening();
    }
    else
    {
        cerr << "Flag no reconocido: " << flag << "\n";
    }
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;
    img->escribir();

    delete img;

    cerr << "Tiempo de CPU: " << elapsed.count() << " segundos\n";

    return 0;
}
