#include "ImagenPPM.h"
#include <pthread.h>
#include <algorithm>
#include <cstring>

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

struct PPMRangeArgs
{
    Pixel **mat;
    Pixel **out;
    int N, M;
    int start_row, end_row;
    int kernel_k;
};

static void *ppm_blur_worker(void *arg)
{
    PPMRangeArgs *a = (PPMRangeArgs *)arg;
    Pixel **mat = a->mat;
    Pixel **out = a->out;
    int N = a->N;
    int M = a->M;
    int k = a->kernel_k;
    for (int i = a->start_row; i < a->end_row; ++i)
    {
        for (int j = 0; j < N; ++j)
        {
            int rSum = 0, gSum = 0, bSum = 0;
            int count = 0;
            for (int di = -k; di <= k; ++di)
            {
                for (int dj = -k; dj <= k; ++dj)
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
            out[i][j].r = rSum / count;
            out[i][j].g = gSum / count;
            out[i][j].b = bSum / count;
        }
    }
    return nullptr;
}

static void *ppm_laplace_worker(void *arg)
{
    static const int kernel[3][3] = {{0, 1, 0},
                                     {1, -4, 1},
                                     {0, 1, 0}};
    PPMRangeArgs *a = (PPMRangeArgs *)arg;
    Pixel **mat = a->mat;
    Pixel **out = a->out;
    int N = a->N;
    int M = a->M;
    for (int i = a->start_row; i < a->end_row; ++i)
    {
        for (int j = 0; j < N; ++j)
        {
            int rSum = 0, gSum = 0, bSum = 0;
            for (int di = -1; di <= 1; ++di)
            {
                for (int dj = -1; dj <= 1; ++dj)
                {
                    int ni = i + di;
                    int nj = j + dj;
                    if (ni >= 0 && ni < M && nj >= 0 && nj < N)
                    {
                        rSum += mat[ni][nj].r * kernel[di + 1][dj + 1];
                        gSum += mat[ni][nj].g * kernel[di + 1][dj + 1];
                        bSum += mat[ni][nj].b * kernel[di + 1][dj + 1];
                    }
                }
            }
            out[i][j].r = std::min(255, std::max(0, rSum));
            out[i][j].g = std::min(255, std::max(0, gSum));
            out[i][j].b = std::min(255, std::max(0, bSum));
        }
    }
    return nullptr;
}

static void *ppm_sharpen_final_worker(void *arg)
{
    PPMRangeArgs *a = (PPMRangeArgs *)arg;
    Pixel **mat = a->mat;
    Pixel **blur = a->out;
    int N = a->N;
    int M = a->M;
    double alpha = 1.5;
    for (int i = a->start_row; i < a->end_row; ++i)
    {
        for (int j = 0; j < N; ++j)
        {
            int r = mat[i][j].r + (int)(alpha * (mat[i][j].r - blur[i][j].r));
            int g = mat[i][j].g + (int)(alpha * (mat[i][j].g - blur[i][j].g));
            int b = mat[i][j].b + (int)(alpha * (mat[i][j].b - blur[i][j].b));

            mat[i][j].r = std::min(255, std::max(0, r));
            mat[i][j].g = std::min(255, std::max(0, g));
            mat[i][j].b = std::min(255, std::max(0, b));
        }
    }
    return nullptr;
}

/* ---------- Filtros paralelos ---------- */

void ImagenPPM::blur()
{
    Pixel **temp = new Pixel *[M];
    for (int i = 0; i < M; i++)
        temp[i] = new Pixel[N];

    int k = 1; // radio 1 (3x3) to be similar to original (used 3x3 blur in PPM)
    int tcount = std::max(1, Imagen::numThreads);
    if (tcount > M)
        tcount = M;

    pthread_t *threads = new pthread_t[tcount];
    PPMRangeArgs *args = new PPMRangeArgs[tcount];

    int base = M / tcount;
    int rem = M % tcount;
    int row = 0;
    for (int t = 0; t < tcount; ++t)
    {
        int rows_for_t = base + (t < rem ? 1 : 0);
        args[t].mat = mat;
        args[t].out = temp;
        args[t].N = N;
        args[t].M = M;
        args[t].kernel_k = k;
        args[t].start_row = row;
        args[t].end_row = row + rows_for_t;
        pthread_create(&threads[t], nullptr, ppm_blur_worker, &args[t]);
        row += rows_for_t;
    }
    for (int t = 0; t < tcount; ++t)
        pthread_join(threads[t], nullptr);

    // copiar temp a mat
    for (int i = 0; i < M; ++i)
        for (int j = 0; j < N; ++j)
            mat[i][j] = temp[i][j];

    for (int i = 0; i < M; i++)
        delete[] temp[i];
    delete[] temp;
    delete[] threads;
    delete[] args;
}

void ImagenPPM::laplace()
{
    Pixel **temp = new Pixel *[M];
    for (int i = 0; i < M; i++)
        temp[i] = new Pixel[N];

    int tcount = std::max(1, Imagen::numThreads);
    if (tcount > M)
        tcount = M;
    pthread_t *threads = new pthread_t[tcount];
    PPMRangeArgs *args = new PPMRangeArgs[tcount];

    int base = M / tcount;
    int rem = M % tcount;
    int row = 0;
    for (int t = 0; t < tcount; ++t)
    {
        int rows_for_t = base + (t < rem ? 1 : 0);
        args[t].mat = mat;
        args[t].out = temp;
        args[t].N = N;
        args[t].M = M;
        args[t].start_row = row;
        args[t].end_row = row + rows_for_t;
        pthread_create(&threads[t], nullptr, ppm_laplace_worker, &args[t]);
        row += rows_for_t;
    }

    for (int t = 0; t < tcount; ++t)
        pthread_join(threads[t], nullptr);

    // copiar temp a mat
    for (int i = 0; i < M; ++i)
        for (int j = 0; j < N; ++j)
            mat[i][j] = temp[i][j];

    for (int i = 0; i < M; i++)
        delete[] temp[i];
    delete[] temp;
    delete[] threads;
    delete[] args;
}

void ImagenPPM::sharpening()
{

    Pixel **temp = new Pixel *[M];
    for (int i = 0; i < M; i++)
        temp[i] = new Pixel[N];

    int k = 1;
    int tcount = std::max(1, Imagen::numThreads);
    if (tcount > M)
        tcount = M;
    pthread_t *threads = new pthread_t[tcount];
    PPMRangeArgs *args = new PPMRangeArgs[tcount];

    int base = M / tcount;
    int rem = M % tcount;
    int row = 0;
    for (int t = 0; t < tcount; ++t)
    {
        int rows_for_t = base + (t < rem ? 1 : 0);
        args[t].mat = mat;
        args[t].out = temp;
        args[t].N = N;
        args[t].M = M;
        args[t].kernel_k = k;
        args[t].start_row = row;
        args[t].end_row = row + rows_for_t;
        pthread_create(&threads[t], nullptr, ppm_blur_worker, &args[t]);
        row += rows_for_t;
    }
    for (int t = 0; t < tcount; ++t)
        pthread_join(threads[t], nullptr);

    row = 0;
    for (int t = 0; t < tcount; ++t)
    {
        int rows_for_t = base + (t < rem ? 1 : 0);
        args[t].mat = mat;
        args[t].out = temp;
        args[t].N = N;
        args[t].M = M;
        args[t].start_row = row;
        args[t].end_row = row + rows_for_t;
        pthread_create(&threads[t], nullptr, ppm_sharpen_final_worker, &args[t]);
        row += rows_for_t;
    }
    for (int t = 0; t < tcount; ++t)
        pthread_join(threads[t], nullptr);

    for (int i = 0; i < M; i++)
        delete[] temp[i];
    delete[] temp;
    delete[] threads;
    delete[] args;
}
