#include "ImagenPGM.h"
#include "ImagenPPM.h"
#include <cstring>

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        cerr << "Uso: " << argv[0] << " <archivo_entrada> <archivo_salida>\n";
        return 1;
    }

    freopen(argv[1], "r", stdin);
    freopen(argv[2], "w", stdout);

    char tipo[3];
    cin >> tipo;

    for (int i = (int)strlen(tipo) - 1; i >= 0; i--)
        cin.putback(tipo[i]);

    Imagen *img = nullptr;
    if (tipo[0] == 'P' && tipo[1] == '2')
        img = new ImagenPGM();
    else if (tipo[0] == 'P' && tipo[1] == '3')
        img = new ImagenPPM();
    else
    {
        cerr << "Formato no soportado." << endl;
        return 1;
    }

    img->leer();
    img->blur();
    img->escribir();

    delete img;
    return 0;
}
