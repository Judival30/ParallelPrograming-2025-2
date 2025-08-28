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

void ImagenPGM::procesar()
{
    // Negativo
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            mat[i][j] = mxVal - mat[i][j];
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
