#include "ImagenPGM.h"
#include <pthread.h>
#include <algorithm>
#include <cstring>
#include <unistd.h>

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

    // Reservar memoria
    mat = new int *[M];
    for (int i = 0; i < M; i++)
        mat[i] = new int[N];

    // Leer pixeles
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            cin >> mat[i][j];
}

void ImagenPGM::escribir()
{
    cout << "P2\n"
         << N << " " << M << "\n"
         << mxVal << "\n";
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
            cout << mat[i][j] << "\n";
        cout << "\n";
    }
}

/* ---------- Helpers para pthreads ---------- */
struct PGMRangeArgs
{
    int **mat;
    int **out; // destino (temp / blur)
    int N, M;
    int start_row, end_row; // inclusive start, exclusive end
    int kernel_k;
};

static void *pgm_blur_worker(void *arg)
{
    PGMRangeArgs *a = (PGMRangeArgs *)arg;
    int **mat = a->mat;
    int **out = a->out;
    int N = a->N;
    int M = a->M;
    int k = a->kernel_k;
    for (int i = a->start_row; i < a->end_row; ++i)
    {
        for (int j = 0; j < N; ++j)
        {
            int cont = 0;
            int sum = 0;
            for (int di = -k; di <= k; ++di)
            {
                for (int dj = -k; dj <= k; ++dj)
                {
                    int ni = i + di, nj = j + dj;
                    if (ni >= 0 && ni < M && nj >= 0 && nj < N)
                    {
                        sum += mat[ni][nj];
                        cont++;
                    }
                }
            }
            out[i][j] = sum / cont;
        }
    }
    return nullptr;
}

static void *pgm_laplace_worker(void *arg)
{
    // kernel 3x3: 0 1 0 / 1 -4 1 / 0 1 0
    static const int kernel[3][3] = {{0, 1, 0}, {1, -4, 1}, {0, 1, 0}};
    PGMRangeArgs *a = (PGMRangeArgs *)arg;
    int **mat = a->mat;
    int **out = a->out;
    int N = a->N;
    int M = a->M;
    int k = 1;
    for (int i = a->start_row; i < a->end_row; ++i)
    {
        for (int j = 0; j < N; ++j)
        {
            int sum = 0;
            for (int di = -k; di <= k; ++di)
            {
                for (int dj = -k; dj <= k; ++dj)
                {
                    int ni = i + di, nj = j + dj;
                    if (ni >= 0 && ni < M && nj >= 0 && nj < N)
                    {
                        sum += mat[ni][nj] * kernel[di + 1][dj + 1];
                    }
                }
            }
            out[i][j] = sum;
        }
    }
    return nullptr;
}

static void *pgm_sharpen_final_worker(void *arg)
{
    // args: mat (original), out (blur), dest is we will write into mat
    PGMRangeArgs *a = (PGMRangeArgs *)arg;
    int **mat = a->mat;  // here mat initially holds original; we will overwrite only assigned rows
    int **blur = a->out; // precomputed blur
    int N = a->N;
    int M = a->M;
    float factor = 2.0f;
    for (int i = a->start_row; i < a->end_row; ++i)
    {
        for (int j = 0; j < N; ++j)
        {
            int detail = mat[i][j] - blur[i][j];
            int val = mat[i][j] + (int)(factor * detail);
            if (val > 255)
                val = 255;
            if (val < 0)
                val = 0;
            mat[i][j] = val;
        }
    }
    return nullptr;
}

/* ---------- Implementaciones de filtros (paralelas) ---------- */

void ImagenPGM::blur()
{
    int **temp = new int *[M];
    for (int i = 0; i < M; i++)
        temp[i] = new int[N];

    int k = 5;
    k /= 2; // radio

    int tcount = std::max(1, Imagen::numThreads);
    if (tcount > M)
        tcount = M;
    pthread_t *threads = new pthread_t[tcount];
    PGMRangeArgs *args = new PGMRangeArgs[tcount];

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
        pthread_create(&threads[t], nullptr, pgm_blur_worker, &args[t]);
        row += rows_for_t;
    }

    for (int t = 0; t < tcount; ++t)
        pthread_join(threads[t], nullptr);

    // copiar temp a mat (se puede hacer paralelo, pero es simple hacer secuencial)
    for (int i = 0; i < M; ++i)
        for (int j = 0; j < N; ++j)
            mat[i][j] = min(255, max(temp[i][j], 0));

    for (int i = 0; i < M; i++)
        delete[] temp[i];
    delete[] temp;
    delete[] threads;
    delete[] args;
}

void ImagenPGM::laplace()
{
    int **temp = new int *[M];
    for (int i = 0; i < M; i++)
        temp[i] = new int[N];

    int tcount = std::max(1, Imagen::numThreads);
    if (tcount > M)
        tcount = M;
    pthread_t *threads = new pthread_t[tcount];
    PGMRangeArgs *args = new PGMRangeArgs[tcount];

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
        pthread_create(&threads[t], nullptr, pgm_laplace_worker, &args[t]);
        row += rows_for_t;
    }

    for (int t = 0; t < tcount; ++t)
        pthread_join(threads[t], nullptr);

    // copiar temp a mat
    for (int i = 0; i < M; ++i)
        for (int j = 0; j < N; ++j)
            mat[i][j] = min(255, max(temp[i][j], 0));

    for (int i = 0; i < M; i++)
        delete[] temp[i];
    delete[] temp;
    delete[] threads;
    delete[] args;
}

void ImagenPGM::sharpening()
{
    // Paso 1: calcular blur en 'blur' (temp)
    int **blur = new int *[M];
    for (int i = 0; i < M; i++)
        blur[i] = new int[N];

    int tcount = std::max(1, Imagen::numThreads);
    if (tcount > M)
        tcount = M;
    pthread_t *threads = new pthread_t[tcount];
    PGMRangeArgs *args = new PGMRangeArgs[tcount];

    int base = M / tcount;
    int rem = M % tcount;
    int row = 0;
    // kernel 3x3 ones, divisor = 9
    int k = 1;
    for (int t = 0; t < tcount; ++t)
    {
        int rows_for_t = base + (t < rem ? 1 : 0);
        args[t].mat = mat;
        args[t].out = blur;
        args[t].N = N;
        args[t].M = M;
        args[t].kernel_k = k;
        args[t].start_row = row;
        args[t].end_row = row + rows_for_t;

        // We'll reuse the blur worker but with k==1: compute average manually here instead of reusing blur worker.
        pthread_create(&threads[t], nullptr, [](void *arg) -> void *
                       {
            PGMRangeArgs *a = (PGMRangeArgs *)arg;
            int **mat = a->mat;
            int **out = a->out;
            int N = a->N;
            int M = a->M;
            int k = a->kernel_k;
            const int kernel[3][3] = {{1,1,1},{1,1,1},{1,1,1}};
            int divisor = 9;
            for (int i = a->start_row; i < a->end_row; ++i)
            {
                for (int j = 0; j < N; ++j)
                {
                    int sum = 0;
                    for (int di = -k; di <= k; ++di)
                    {
                        for (int dj = -k; dj <= k; ++dj)
                        {
                            int ni = i + di, nj = j + dj;
                            if (ni >= 0 && ni < M && nj >= 0 && nj < N)
                            {
                                sum += mat[ni][nj] * kernel[di + k][dj + k];
                            }
                        }
                    }
                    out[i][j] = sum / divisor;
                }
            }
            return nullptr; }, &args[t]);

        row += rows_for_t;
    }

    for (int t = 0; t < tcount; ++t)
        pthread_join(threads[t], nullptr);

    // Paso 2: aplicar realce en paralelo (leer mat y blur, escribir mat)
    // Reutilizamos args/threads ranges
    row = 0;
    for (int t = 0; t < tcount; ++t)
    {
        int rows_for_t = base + (t < rem ? 1 : 0);
        args[t].mat = mat;
        args[t].out = blur;
        args[t].N = N;
        args[t].M = M;
        args[t].start_row = row;
        args[t].end_row = row + rows_for_t;
        pthread_create(&threads[t], nullptr, pgm_sharpen_final_worker, &args[t]);
        row += rows_for_t;
    }
    for (int t = 0; t < tcount; ++t)
        pthread_join(threads[t], nullptr);

    for (int i = 0; i < M; i++)
        delete[] blur[i];
    delete[] blur;
    delete[] threads;
    delete[] args;
}
