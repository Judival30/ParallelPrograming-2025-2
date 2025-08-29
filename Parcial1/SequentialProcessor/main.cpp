#include "ImagenPGM.h"
#include "ImagenPPM.h"
#include <cstring>
#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
    if (argc < 5)
    {
        cerr << "Uso: " << argv[0] << " <entrada> <salida> --f <filtro>\n";
        return 1;
    }

    // Redirigir entrada y salida
    freopen(argv[1], "r", stdin);
    freopen(argv[2], "w", stdout);

    // Leer tipo de archivo (P2 o P3)
    char tipo[3];
    cin >> tipo;

    // Volver a meter el tipo al flujo
    for (int i = (int)strlen(tipo) - 1; i >= 0; i--)
        cin.putback(tipo[i]);

    // Crear objeto imagen
    Imagen *img = nullptr;
    if (tipo[0] == 'P' && tipo[1] == '2')
        img = new ImagenPGM();
    else if (tipo[0] == 'P' && tipo[1] == '3')
        img = new ImagenPPM();
    else
    {
        cerr << "Formato no soportado: " << tipo << "\n";
        return 1;
    }

    img->leer();

    char *flag = argv[3];
    char *filter = argv[4];

    if (strcmp(flag, "--f") == 0)
    {
        if (strcmp(filter, "blur") == 0)
            img->blur();
        else if (strcmp(filter, "sharpening") == 0)
            img->sharpening();
        else if (strcmp(filter, "laplace") == 0)
            img->laplace();
        else
            cerr << "Filtro no reconocido: " << filter << "\n";
    }
    else
    {
        cerr << "Flag no reconocido: " << flag << "\n";
    }

    img->escribir();

    delete img;
    return 0;
}
