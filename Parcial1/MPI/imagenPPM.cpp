#include "imagenPPM.h"
#include <algorithm>
#include <vector>
using namespace std;

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
    string tipo;
    cin >> tipo;
    cin >> N >> M >> mxVal;
    mat = new Pixel *[M];
    for (int i = 0; i < M; i++)
    {
        mat[i] = new Pixel[N];
        for (int j = 0; j < N; j++)
        {
            cin >> mat[i][j].r >> mat[i][j].g >> mat[i][j].b;
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
            cout << mat[i][j].r << " " << mat[i][j].g << " " << mat[i][j].b << " ";
        }
        cout << "\n";
    }
}

void ImagenPPM::blur()
{
    vector<vector<Pixel>> temp(M, vector<Pixel>(N));
    int dirs[9][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 0}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
        {
            int r = 0, g = 0, b = 0, c = 0;
            for (auto &d : dirs)
            {
                int ni = i + d[0], nj = j + d[1];
                if (ni >= 0 && ni < M && nj >= 0 && nj < N)
                {
                    r += mat[ni][nj].r;
                    g += mat[ni][nj].g;
                    b += mat[ni][nj].b;
                    c++;
                }
            }
            temp[i][j].r = r / c;
            temp[i][j].g = g / c;
            temp[i][j].b = b / c;
        }
    }
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            mat[i][j] = temp[i][j];
}

void ImagenPPM::laplace()
{
    vector<vector<Pixel>> temp(M, vector<Pixel>(N));
    int kernel[3][3] = {{0, 1, 0}, {1, -4, 1}, {0, 1, 0}};
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
        {
            int r = 0, g = 0, b = 0;
            for (int di = -1; di <= 1; di++)
            {
                for (int dj = -1; dj <= 1; dj++)
                {
                    int ni = i + di, nj = j + dj;
                    if (ni >= 0 && ni < M && nj >= 0 && nj < N)
                    {
                        r += mat[ni][nj].r * kernel[di + 1][dj + 1];
                        g += mat[ni][nj].g * kernel[di + 1][dj + 1];
                        b += mat[ni][nj].b * kernel[di + 1][dj + 1];
                    }
                }
            }
            temp[i][j].r = max(0, min(255, r));
            temp[i][j].g = max(0, min(255, g));
            temp[i][j].b = max(0, min(255, b));
        }
    }
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            mat[i][j] = temp[i][j];
}

void ImagenPPM::sharpening()
{
    vector<vector<Pixel>> temp(M, vector<Pixel>(N));
    int kernel[3][3] = {{0, -1, 0}, {-1, 5, -1}, {0, -1, 0}};
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
        {
            int r = 0, g = 0, b = 0;
            for (int di = -1; di <= 1; di++)
            {
                for (int dj = -1; dj <= 1; dj++)
                {
                    int ni = i + di, nj = j + dj;
                    if (ni >= 0 && ni < M && nj >= 0 && nj < N)
                    {
                        r += mat[ni][nj].r * kernel[di + 1][dj + 1];
                        g += mat[ni][nj].g * kernel[di + 1][dj + 1];
                        b += mat[ni][nj].b * kernel[di + 1][dj + 1];
                    }
                }
            }
            temp[i][j].r = max(0, min(255, r));
            temp[i][j].g = max(0, min(255, g));
            temp[i][j].b = max(0, min(255, b));
        }
    }
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            mat[i][j] = temp[i][j];
}
