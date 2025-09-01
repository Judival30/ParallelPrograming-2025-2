#include <mpi.h>
#include <iostream>
#include <vector>
#include <cstring>
#include <fstream>
#include "imagenPGM.h"

using namespace std;

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc < 3)
    {
        if (rank == 0)
            cerr << "Uso: mpirun -np <N> ./filtro_mpi <in.pgm> <out_base>\n";
        MPI_Finalize();
        return 1;
    }

    if (rank == 0)
    {
        // MASTER
        freopen(argv[1], "r", stdin);
        ImagenPGM *img = new ImagenPGM();
        img->leer();

        // Tiempo inicial
        double t_start = MPI_Wtime();

        // Broadcast dimensiones a los esclavos
        int dims[3] = {img->M, img->N, img->mxVal};
        for (int p = 1; p < size; p++)
            MPI_Send(dims, 3, MPI_INT, p, 0, MPI_COMM_WORLD);

        // Partición de filas entre (size-1) esclavos
        int workers = size - 1;
        if (workers <= 0)
        {
            cerr << "Se requieren al menos 2 procesos (1 master + 1 esclavo)\n";
            delete img;
            MPI_Finalize();
            return 1;
        }
        int base = img->M / workers;
        int rem = img->M % workers;

        // Buffers de salida (uno por filtro)
        vector<vector<int>> matBlur(img->M, vector<int>(img->N));
        vector<vector<int>> matLaplace(img->M, vector<int>(img->N));
        vector<vector<int>> matSharp(img->M, vector<int>(img->N));

        // Enviar bloques
        int offset = 0;
        for (int p = 1; p <= workers; p++)
        {
            int rows = base + (p <= rem ? 1 : 0);
            for (int i = 0; i < rows; i++)
            {
                MPI_Send(img->mat[offset + i], img->N, MPI_INT, p, 0, MPI_COMM_WORLD);
            }
            offset += rows;
        }

        // Recibir resultados (3 bloques por proceso: blur/laplace/sharpen)
        offset = 0;
        for (int p = 1; p <= workers; p++)
        {
            int rows = base + (p <= rem ? 1 : 0);
            for (int i = 0; i < rows; i++)
            {
                MPI_Recv(matBlur[offset + i].data(), img->N, MPI_INT, p, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(matLaplace[offset + i].data(), img->N, MPI_INT, p, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(matSharp[offset + i].data(), img->N, MPI_INT, p, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            offset += rows;
        }

        // Tiempo final
        double t_end = MPI_Wtime();
        cout << "Tiempo total (lectura -> escritura): "
             << (t_end - t_start) << " segundos" << endl;

        // Escribir 3 archivos PGM
        const string baseName = argv[2];
        {
            ofstream fout(baseName + "_blur.pgm");
            fout << "P2\n"
                 << img->N << " " << img->M << "\n"
                 << img->mxVal << "\n";
            for (int i = 0; i < img->M; i++)
            {
                for (int j = 0; j < img->N; j++)
                    fout << matBlur[i][j] << " ";
                fout << "\n";
            }
        }
        {
            ofstream fout(baseName + "_laplace.pgm");
            fout << "P2\n"
                 << img->N << " " << img->M << "\n"
                 << img->mxVal << "\n";
            for (int i = 0; i < img->M; i++)
            {
                for (int j = 0; j < img->N; j++)
                    fout << matLaplace[i][j] << " ";
                fout << "\n";
            }
        }
        {
            ofstream fout(baseName + "_sharpen.pgm");
            fout << "P2\n"
                 << img->N << " " << img->M << "\n"
                 << img->mxVal << "\n";
            for (int i = 0; i < img->M; i++)
            {
                for (int j = 0; j < img->N; j++)
                    fout << matSharp[i][j] << " ";
                fout << "\n";
            }
        }

        delete img;
    }
    else
    {
        // SLAVES
        int dims[3];
        MPI_Recv(dims, 3, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        int M = dims[0], N = dims[1], mxVal = dims[2];

        int workers = size - 1;
        int base = M / workers;
        int rem = M % workers;
        int rows = base + (rank <= rem ? 1 : 0);

        // Recibir bloque local
        ImagenPGM local;
        local.M = rows;
        local.N = N;
        local.mxVal = mxVal;
        local.mat = new int *[rows];
        for (int i = 0; i < rows; i++)
        {
            local.mat[i] = new int[N];
            MPI_Recv(local.mat[i], N, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        // Crear copias profundas para cada filtro (cada una con su propio mat)
        ImagenPGM blurImg;
        blurImg.M = rows;
        blurImg.N = N;
        blurImg.mxVal = mxVal;
        blurImg.mat = new int *[rows];
        for (int i = 0; i < rows; i++)
        {
            blurImg.mat[i] = new int[N];
            memcpy(blurImg.mat[i], local.mat[i], N * sizeof(int));
        }

        ImagenPGM laplaceImg;
        laplaceImg.M = rows;
        laplaceImg.N = N;
        laplaceImg.mxVal = mxVal;
        laplaceImg.mat = new int *[rows];
        for (int i = 0; i < rows; i++)
        {
            laplaceImg.mat[i] = new int[N];
            memcpy(laplaceImg.mat[i], local.mat[i], N * sizeof(int));
        }

        ImagenPGM sharpImg;
        sharpImg.M = rows;
        sharpImg.N = N;
        sharpImg.mxVal = mxVal;
        sharpImg.mat = new int *[rows];
        for (int i = 0; i < rows; i++)
        {
            sharpImg.mat[i] = new int[N];
            memcpy(sharpImg.mat[i], local.mat[i], N * sizeof(int));
        }

        // Aplicar filtros
        blurImg.blur();
        laplaceImg.laplace();
        sharpImg.sharpening();

        // Enviar las 3 versiones al master (tags 0,1,2)
        for (int i = 0; i < rows; i++)
        {
            MPI_Send(blurImg.mat[i], N, MPI_INT, 0, 0, MPI_COMM_WORLD);
            MPI_Send(laplaceImg.mat[i], N, MPI_INT, 0, 1, MPI_COMM_WORLD);
            MPI_Send(sharpImg.mat[i], N, MPI_INT, 0, 2, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();
    return 0;
}
