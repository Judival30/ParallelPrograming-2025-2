#include "imagenPGM.h"
#include <omp.h>
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <cstring>
using namespace std;

ImagenPGM::~ImagenPGM()
{
    if (mat)
    {
        for (int i = 0; i < M; i++)
            delete[] mat[i];
        delete[] mat;
        mat = nullptr;
    }
}

void ImagenPGM::leer()
{
    char tipo[3];
    cin >> tipo;
    if (tipo[0] != 'P' || tipo[1] != '2')
    {
        cerr << "Formato incorrecto, se esperaba P2 (PGM)." << endl;
        exit(1);
    }

    // Ignorar comentarios
    while (cin.peek() == '#')
    {
        char buffer[256];
        cin.getline(buffer, 256);
    }

    cin >> N >> M >> mxVal;

    mat = new int *[M];
    for (int i = 0; i < M; i++)
        mat[i] = new int[N];

    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            cin >> mat[i][j];
}

void ImagenPGM::escribir(FILE *out)
{
    if (!out)
        return;
    fprintf(out, "P2\n%d %d\n%d\n", N, M, mxVal);
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
        {
            fprintf(out, "%d\n", mat[i][j]);
        }
        fprintf(out, "\n");
    }
}

void ImagenPGM::blur()
{
    int **temp = new int *[M];
    for (int i = 0; i < M; i++)
        temp[i] = new int[N];

    int k = 5;
    k /= 2; // k = 2

#pragma omp parallel for schedule(static)
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
        {
            int cont = 0, sum = 0;
            for (int di = -k; di <= k; di++)
            {
                for (int dj = -k; dj <= k; dj++)
                {
                    int ni = i + di, nj = j + dj;
                    if (ni >= 0 && ni < M && nj >= 0 && nj < N)
                    {
                        sum += mat[ni][nj];
                        cont++;
                    }
                }
            }
            temp[i][j] = (cont > 0) ? (sum / cont) : mat[i][j];
        }
    }

#pragma omp parallel for schedule(static)
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            mat[i][j] = temp[i][j];

    for (int i = 0; i < M; i++)
        delete[] temp[i];
    delete[] temp;
}

void ImagenPGM::laplace()
{
    int **temp = new int *[M];
    for (int i = 0; i < M; i++)
        temp[i] = new int[N];

    int kernel[3][3] = {{0, 1, 0}, {1, -4, 1}, {0, 1, 0}};

#pragma omp parallel for schedule(static)
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
        {
            int sum = 0;
            for (int di = -1; di <= 1; di++)
            {
                for (int dj = -1; dj <= 1; dj++)
                {
                    int ni = i + di, nj = j + dj;
                    if (ni >= 0 && ni < M && nj >= 0 && nj < N)
                        sum += mat[ni][nj] * kernel[di + 1][dj + 1];
                }
            }
            // normalizar/clamp al rango válido
            int val = sum;
            if (val < 0)
                val = 0;
            if (val > mxVal)
                val = mxVal;
            temp[i][j] = val;
        }
    }

#pragma omp parallel for schedule(static)
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            mat[i][j] = temp[i][j];

    for (int i = 0; i < M; i++)
        delete[] temp[i];
    delete[] temp;
}

void ImagenPGM::sharpening()
{
    int **blur = new int *[M];
    for (int i = 0; i < M; i++)
        blur[i] = new int[N];

    int kernel[3][3] = {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}};
    int divisor = 9;

#pragma omp parallel for schedule(static)
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
        {
            int sum = 0;
            for (int di = -1; di <= 1; di++)
            {
                for (int dj = -1; dj <= 1; dj++)
                {
                    int ni = i + di, nj = j + dj;
                    if (ni >= 0 && ni < M && nj >= 0 && nj < N)
                        sum += mat[ni][nj] * kernel[di + 1][dj + 1];
                }
            }
            blur[i][j] = sum / divisor;
        }
    }

    float factor = 2.0f;

#pragma omp parallel for schedule(static)
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
        {
            int detail = mat[i][j] - blur[i][j];
            int val = static_cast<int>(mat[i][j] + factor * detail);
            if (val > mxVal)
                val = mxVal;
            if (val < 0)
                val = 0;
            mat[i][j] = val;
        }
    }

    for (int i = 0; i < M; i++)
        delete[] blur[i];
    delete[] blur;
}

Imagen *ImagenPGM::clonar()
{
    ImagenPGM *copia = new ImagenPGM();
    copia->N = N;
    copia->M = M;
    copia->mxVal = mxVal;
    copia->mat = new int *[M];
    for (int i = 0; i < M; i++)
    {
        copia->mat[i] = new int[N];
        for (int j = 0; j < N; j++)
            copia->mat[i][j] = mat[i][j];
    }
    return copia;
}
