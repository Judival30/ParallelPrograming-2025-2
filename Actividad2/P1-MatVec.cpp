#include <bits/stdc++.h>
#include <pthread.h>
using namespace std;

const int M = 4;
const int N = 4;
const int THREADS = 2;

vector<vector<double>> A(M, vector<double>(N));
vector<double> x(N), y(M, 0.0);

int thread_count = THREADS;

void *mat_vect(void *rank)
{
    long r = (long)rank;
    int range = M / thread_count;
    int ini = r * range;
    int fin = (r + 1) * range - 1;

    for (int i = ini; i <= fin; i++)
    {
        y[i] = 0.0;
        for (int j = 0; j < N; j++)
        {
            y[i] += A[i][j] * x[j];
        }
    }
    return nullptr;
}

int main()
{
    pthread_t threads[THREADS];

    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            A[i][j] = i + j * 4;

    for (int i = 0; i < N; i++)
        x[i] = i * 3;

    for (long t = 0; t < THREADS; t++)
        pthread_create(&threads[t], nullptr, mat_vect, (void *)t);

    for (int t = 0; t < THREADS; t++)
        pthread_join(threads[t], nullptr);

    cout << "Resultado y = A * x :" << endl;
    for (int i = 0; i < M; i++)
        cout << y[i] << endl;

    return 0;
}
