#include "ImagenPGM.h"

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

void ImagenPGM::blur()
{
    int **temp = new int *[M];
    for (int i = 0; i < M; i++)
        temp[i] = new int[N];
    int k = 5;
    int dx[4] = {-1, 1, 0, 0};
    int dy[4] = {0, 0, -1, 1};
    k /= 2;
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
        {
            int cont = 0;
            int sum = 0;
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
            temp[i][j] = sum / cont;
        }
    }
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
    int k = 5;

    int kernel[3][3] = {{0, 1, 0},
                        {1, -4, 1},
                        {0, 1, 0}};
    k = 1;
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
        {

            int sum = 0;
            for (int di = -k; di <= k; di++)
            {
                for (int dj = -k; dj <= k; dj++)
                {
                    int ni = i + di, nj = j + dj;
                    if (ni >= 0 && ni < M && nj >= 0 && nj < N)
                    {
                        sum += mat[ni][nj] * kernel[di + 1][dj + 1];
                    }
                }
            }
            temp[i][j] = sum;
        }
    }
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

    int kernel[3][3] = {{1, 1, 1},
                        {1, 1, 1},
                        {1, 1, 1}};
    int k = 1;
    int divisor = 9; // suma del kernel

    // Paso 1: calcular imagen suavizada
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
        {
            int sum = 0;
            for (int di = -k; di <= k; di++)
            {
                for (int dj = -k; dj <= k; dj++)
                {
                    int ni = i + di, nj = j + dj;
                    if (ni >= 0 && ni < M && nj >= 0 && nj < N)
                    {
                        sum += mat[ni][nj] * kernel[di + k][dj + k];
                    }
                }
            }
            blur[i][j] = sum / divisor;
        }
    }

    float factor = 2.0; // controla la intensidad del sharpen
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
        {
            int detail = mat[i][j] - blur[i][j];
            int val = mat[i][j] + factor * detail;

            if (val > 255)
                val = 255;
            if (val < 0)
                val = 0;
            mat[i][j] = val;
        }
    }

    // Liberar memoria
    for (int i = 0; i < M; i++)
        delete[] blur[i];
    delete[] blur;
}
