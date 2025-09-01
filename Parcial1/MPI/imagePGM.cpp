#include "imagenPGM.h"
#include <algorithm>
#include <vector>
using namespace std;

ImagenPGM::~ImagenPGM()
{
    if (mat)
    {
        for (int i = 0; i < M; i++)
            delete[] mat[i];
        delete[] mat;
    }
}

void ImagenPGM::leer()
{
    string tipo;
    cin >> tipo;
    cin >> N >> M >> mxVal;
    mat = new int *[M];
    for (int i = 0; i < M; i++)
    {
        mat[i] = new int[N];
        for (int j = 0; j < N; j++)
            cin >> mat[i][j];
    }
}

void ImagenPGM::escribir()
{
    cout << "P2\n"
         << N << " " << M << "\n"
         << mxVal << "\n";
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
            cout << mat[i][j] << " ";
        cout << "\n";
    }
}

void ImagenPGM::blur()
{
    vector<vector<int>> temp(M, vector<int>(N));
    int dirs[9][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 0}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
        {
            int sum = 0, count = 0;
            for (auto &d : dirs)
            {
                int ni = i + d[0], nj = j + d[1];
                if (ni >= 0 && ni < M && nj >= 0 && nj < N)
                {
                    sum += mat[ni][nj];
                    count++;
                }
            }
            temp[i][j] = sum / count;
        }
    }
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            mat[i][j] = temp[i][j];
}

void ImagenPGM::laplace()
{
    vector<vector<int>> temp(M, vector<int>(N));
    int kernel[3][3] = {{0, 1, 0}, {1, -4, 1}, {0, 1, 0}};
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
                    {
                        sum += mat[ni][nj] * kernel[di + 1][dj + 1];
                    }
                }
            }
            temp[i][j] = max(0, min(255, sum));
        }
    }
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            mat[i][j] = temp[i][j];
}

void ImagenPGM::sharpening()
{
    vector<vector<int>> temp(M, vector<int>(N));
    int kernel[3][3] = {{0, -1, 0}, {-1, 5, -1}, {0, -1, 0}};
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
                    {
                        sum += mat[ni][nj] * kernel[di + 1][dj + 1];
                    }
                }
            }
            temp[i][j] = max(0, min(255, sum));
        }
    }
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            mat[i][j] = temp[i][j];
}
