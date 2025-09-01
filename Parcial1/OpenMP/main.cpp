#include "imagenPGM.h"
#include "imagenPPM.h"
#include <cstring>
#include <iostream>
#include <chrono>
#include <omp.h>
#include <cstdio>
using namespace std;

int main(int argc, char *argv[])
{
    // Uso: ./filterer entrada salidaBase
    // Genera salidaBase_blur.(pgm|ppm), salidaBase_laplace.(pgm|ppm), salidaBase_sharpening.(pgm|ppm)
    if (argc < 3)
    {
        cerr << "Uso: " << argv[0] << " entrada salidaBase\n";
        return 1;
    }

    auto start = chrono::high_resolution_clock::now();

    // redirigir stdin a archivo de entrada para usar cin en leer()
    if (!freopen(argv[1], "r", stdin))
    {
        perror("Error abriendo archivo de entrada");
        return 1;
    }

    char tipo[3];
    cin >> tipo;
    // devolver la cabecera leída al stream para que leer() la procese
    for (int i = (int)strlen(tipo) - 1; i >= 0; i--)
        cin.putback(tipo[i]);

    Imagen *imgOriginal = nullptr;
    const char *ext;

    if (tipo[0] == 'P' && tipo[1] == '2')
    {
        imgOriginal = new ImagenPGM();
        ext = ".pgm";
    }
    else if (tipo[0] == 'P' && tipo[1] == '3')
    {
        imgOriginal = new ImagenPPM();
        ext = ".ppm";
    }
    else
    {
        cerr << "Formato no soportado\n";
        return 1;
    }

    imgOriginal->leer();

    Imagen *imgBlur = imgOriginal->clonar();
    Imagen *imgLaplace = imgOriginal->clonar();
    Imagen *imgSharp = imgOriginal->clonar();

    char outBlur[256], outLaplace[256], outSharp[256];
    strcpy(outBlur, argv[2]);
    strcat(outBlur, "_blur");
    strcat(outBlur, ext);
    strcpy(outLaplace, argv[2]);
    strcat(outLaplace, "_laplace");
    strcat(outLaplace, ext);
    strcpy(outSharp, argv[2]);
    strcat(outSharp, "_sharpening");
    strcat(outSharp, ext);

#pragma omp parallel sections
    {
#pragma omp section
        {
            FILE *fout = fopen(outBlur, "w");
            if (!fout)
            {
                perror("fopen");
            }
            imgBlur->blur();
            imgBlur->escribir(fout);
            if (fout)
                fclose(fout);
        }
#pragma omp section
        {
            FILE *fout = fopen(outLaplace, "w");
            if (!fout)
            {
                perror("fopen");
            }
            imgLaplace->laplace();
            imgLaplace->escribir(fout);
            if (fout)
                fclose(fout);
        }
#pragma omp section
        {
            FILE *fout = fopen(outSharp, "w");
            if (!fout)
            {
                perror("fopen");
            }
            imgSharp->sharpening();
            imgSharp->escribir(fout);
            if (fout)
                fclose(fout);
        }
    }

    delete imgOriginal;
    delete imgBlur;
    delete imgLaplace;
    delete imgSharp;

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;
    cerr << "Tiempo total: " << elapsed.count() << " segundos\n";

    return 0;
}
