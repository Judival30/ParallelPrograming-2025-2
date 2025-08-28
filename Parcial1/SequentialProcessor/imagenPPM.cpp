#include "ImagenPPM.h"

ImagenPPM::~ImagenPPM()
{
    if (mat)
    {
        for (int i = 0; i < M; i++)
            delete[] mat[i];
        delete[] mat;
    }
}

void ImagenPPM::leer()
{
    char tipo[3];
    cin >> tipo;
    if (tipo[0] != 'P' || tipo[1] != '3')
    {
        cerr << "Formato incorrecto, se esperaba P3 (PPM)." << endl;
        exit(1);
    }

    // Ignorar comentarios
    while (cin.peek() == '#')
    {
        char buffer[256];
        cin.getline(buffer, 256);
    }

    cin >> N >> M >> mxVal;

    // Reservar memoria
    mat = new Pixel *[M];
    for (int i = 0; i < M; i++)
        mat[i] = new Pixel[N];

    // Leer pixeles
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            cin >> mat[i][j].r >> mat[i][j].g >> mat[i][j].b;
}

void ImagenPPM::procesar()
{
    // Convertir a escala de grises
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
        {
            int gris = (mat[i][j].r + mat[i][j].g + mat[i][j].b) / 3;
            mat[i][j].r = mat[i][j].g = mat[i][j].b = gris;
        }
    }
}

void ImagenPPM::escribir()
{
    cout << "P3\n"
         << N << " " << M << "\n"
         << mxVal << "\n";
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
        {
            cout << mat[i][j].r << " "
                 << mat[i][j].g << " "
                 << mat[i][j].b << " ";
        }
        cout << "\n";
    }
}

void ImagenPPM::blur()
{
    // Aplicar un desenfoque simple (blur) usando un filtro 3x3
    Pixel **temp = new Pixel *[M];
    for (int i = 0; i < M; i++)
        temp[i] = new Pixel[N];

    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
        {
            int rSum = 0, gSum = 0, bSum = 0;
            int count = 0;

            // Recorrer la vecindad 3x3
            for (int di = -1; di <= 1; di++)
            {
                for (int dj = -1; dj <= 1; dj++)
                {
                    int ni = i + di;
                    int nj = j + dj;
                    if (ni >= 0 && ni < M && nj >= 0 && nj < N)
                    {
                        rSum += mat[ni][nj].r;
                        gSum += mat[ni][nj].g;
                        bSum += mat[ni][nj].b;
                        count++;
                    }
                }
            }

            temp[i][j].r = rSum / count;
            temp[i][j].g = gSum / count;
            temp[i][j].b = bSum / count;
        }
    }

    // Copiar el resultado de vuelta a la matriz original
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            mat[i][j] = temp[i][j];

    // Liberar memoria temporal
    for (int i = 0; i < M; i++)
        delete[] temp[i];
    delete[] temp;
}
