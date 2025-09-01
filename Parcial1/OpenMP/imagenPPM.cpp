#include "imagenPPM.h"
#include <omp.h>
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <cstring>
using namespace std;

ImagenPPM::~ImagenPPM()
{
    if (mat)
    {
        for (int i = 0; i < M; i++)
            delete[] mat[i];
        delete[] mat;
        mat = nullptr;
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

    while (cin.peek() == '#')
    {
        char buffer[256];
        cin.getline(buffer, 256);
    }

    cin >> N >> M >> mxVal;

    mat = new Pixel *[M];
    for (int i = 0; i < M; i++)
        mat[i] = new Pixel[N];

    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            cin >> mat[i][j].r >> mat[i][j].g >> mat[i][j].b;
}

void ImagenPPM::escribir(FILE *out)
{
    if (!out)
        return;
    fprintf(out, "P3\n%d %d\n%d\n", N, M, mxVal);
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
        {
            fprintf(out, "%d %d %d ", mat[i][j].r, mat[i][j].g, mat[i][j].b);
        }
        fprintf(out, "\n");
    }
}

void ImagenPPM::blur()
{
    Pixel **temp = new Pixel *[M];
    for (int i = 0; i < M; i++)
        temp[i] = new Pixel[N];

#pragma omp parallel for schedule(static)
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
        {
            int rSum = 0, gSum = 0, bSum = 0, count = 0;
            for (int di = -1; di <= 1; di++)
            {
                for (int dj = -1; dj <= 1; dj++)
                {
                    int ni = i + di, nj = j + dj;
                    if (ni >= 0 && ni < M && nj >= 0 && nj < N)
                    {
                        rSum += mat[ni][nj].r;
                        gSum += mat[ni][nj].g;
                        bSum += mat[ni][nj].b;
                        count++;
                    }
                }
            }
            if (count == 0)
                count = 1;
            temp[i][j].r = rSum / count;
            temp[i][j].g = gSum / count;
            temp[i][j].b = bSum / count;
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

void ImagenPPM::laplace()
{
    Pixel **temp = new Pixel *[M];
    for (int i = 0; i < M; i++)
        temp[i] = new Pixel[N];

    int kernel[3][3] = {{0, 1, 0}, {1, -4, 1}, {0, 1, 0}};

#pragma omp parallel for schedule(static)
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
        {
            int rSum = 0, gSum = 0, bSum = 0;
            for (int di = -1; di <= 1; di++)
            {
                for (int dj = -1; dj <= 1; dj++)
                {
                    int ni = i + di, nj = j + dj;
                    if (ni >= 0 && ni < M && nj >= 0 && nj < N)
                    {
                        rSum += mat[ni][nj].r * kernel[di + 1][dj + 1];
                        gSum += mat[ni][nj].g * kernel[di + 1][dj + 1];
                        bSum += mat[ni][nj].b * kernel[di + 1][dj + 1];
                    }
                }
            }
            temp[i][j].r = std::min(mxVal, std::max(0, rSum));
            temp[i][j].g = std::min(mxVal, std::max(0, gSum));
            temp[i][j].b = std::min(mxVal, std::max(0, bSum));
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

void ImagenPPM::sharpening()
{
    Pixel **temp = new Pixel *[M];
    for (int i = 0; i < M; i++)
        temp[i] = new Pixel[N];

    int kernel[3][3] = {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}};
    int divisor = 9;

#pragma omp parallel for schedule(static)
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
        {
            int rSum = 0, gSum = 0, bSum = 0;
            for (int di = -1; di <= 1; di++)
            {
                for (int dj = -1; dj <= 1; dj++)
                {
                    int ni = i + di, nj = j + dj;
                    if (ni >= 0 && ni < M && nj >= 0 && nj < N)
                    {
                        rSum += mat[ni][nj].r * kernel[di + 1][dj + 1];
                        gSum += mat[ni][nj].g * kernel[di + 1][dj + 1];
                        bSum += mat[ni][nj].b * kernel[di + 1][dj + 1];
                    }
                }
            }
            temp[i][j].r = rSum / divisor;
            temp[i][j].g = gSum / divisor;
            temp[i][j].b = bSum / divisor;
        }
    }

    double alpha = 1.5;
#pragma omp parallel for schedule(static)
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
        {
            int r = (int)(mat[i][j].r + alpha * (mat[i][j].r - temp[i][j].r));
            int g = (int)(mat[i][j].g + alpha * (mat[i][j].g - temp[i][j].g));
            int b = (int)(mat[i][j].b + alpha * (mat[i][j].b - temp[i][j].b));
            mat[i][j].r = std::min(mxVal, std::max(0, r));
            mat[i][j].g = std::min(mxVal, std::max(0, g));
            mat[i][j].b = std::min(mxVal, std::max(0, b));
        }
    }

    for (int i = 0; i < M; i++)
        delete[] temp[i];
    delete[] temp;
}

Imagen *ImagenPPM::clonar()
{
    ImagenPPM *copia = new ImagenPPM();
    copia->N = N;
    copia->M = M;
    copia->mxVal = mxVal;
    copia->mat = new Pixel *[M];
    for (int i = 0; i < M; i++)
    {
        copia->mat[i] = new Pixel[N];
        for (int j = 0; j < N; j++)
            copia->mat[i][j] = mat[i][j];
    }
    return copia;
}
