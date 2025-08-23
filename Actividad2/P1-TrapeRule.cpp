#include <bits/stdc++.h>
#include <pthread.h>
using namespace std;

const int THREADS = 2;
long n = 1024;            // número de trapecios
double a = 0.0, b = M_PI; // intervalo
double resultado = 0.0;  
pthread_mutex_t mut;


double f(double x) { return sin(x); }

void *worker(void *rank)
{
    long id = (long)rank;
    long local_n = n / THREADS;
    double h = (b - a) / n;

    double ini = a + id * local_n * h;
    double fin = ini + local_n * h;

    double parcial = (f(ini) + f(fin)) / 2.0;
    for (long i = 1; i < local_n; i++)
    {
        parcial += f(ini + i * h);
    }
    parcial *= h;

    pthread_mutex_lock(&mut);
    resultado += parcial;
    pthread_mutex_unlock(&mut);

    return nullptr;
}

int main()
{
    pthread_mutex_init(&mut, nullptr);
    vector<pthread_t> hilos(THREADS);

    for (long t = 0; t < THREADS; t++)
        pthread_create(&hilos[t], nullptr, worker, (void *)t);

    for (int t = 0; t < THREADS; t++)
        pthread_join(hilos[t], nullptr);

    cout << " Método del trapecio paralelo\n";
    cout << fixed << setprecision(6);
    cout << "Integral en [" << a << ", " << b << "] ≈ " << resultado << "\n";
    cout << "Error absoluto: " << abs(resultado - 2) << endl;
    pthread_mutex_destroy(&mut);
    return 0;
}


