#include <bits/stdc++.h>
#include <pthread.h>
#include <unistd.h>

using namespace std;
typedef long long lint;

vector<lint> A;
lint ans = 0;
pthread_mutex_t mut;

struct Rango
{
    lint ini;
    lint fin;
};

void *sumRange(void *arg)
{
    Rango *r = (Rango *)arg;
    lint cont = 0;
    for (int i = r->ini; i < r->fin; i++)
        cont += A[i];

    pthread_mutex_lock(&mut);
    ans += cont;
    pthread_mutex_unlock(&mut);
    return nullptr;
}

int main()
{
    lint N = 1e7;
    A.resize(N);

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<lint> dist(1, 100);

    for (lint i = 0; i < N; i++)
        A[i] = dist(gen);

    lint Nthreads = 4;
    lint range = (N + Nthreads - 1) / Nthreads;

    pthread_mutex_init(&mut, nullptr);
    pthread_t threads[Nthreads];
    vector<Rango> rangos(Nthreads);

    clock_t start_parallel = clock();

    for (lint i = 0; i < Nthreads; i++)
    {
        lint ini = i * range;
        lint fin = min(N, ini + range);
        rangos[i] = {ini, fin};
        pthread_create(&threads[i], nullptr, sumRange, (void *)&rangos[i]);
    }

    for (lint i = 0; i < Nthreads; i++)
        pthread_join(threads[i], nullptr);

    clock_t end_parallel = clock();
    double cpu_time_parallel = double(end_parallel - start_parallel) / CLOCKS_PER_SEC;

    pthread_mutex_destroy(&mut);

    cout << "La suma total (paralelo) es: " << ans << endl;
    cout << "Tiempo CPU paralelo: " << cpu_time_parallel << " segundos\n";

    clock_t start_sequential = clock();

    lint ans2 = 0;
    for (lint i = 0; i < N; i++)
        ans2 += A[i];

    clock_t end_sequential = clock();
    double cpu_time_sequential = double(end_sequential - start_sequential) / CLOCKS_PER_SEC;

    cout << "Verificación secuencial: " << ans2 << endl;
    cout << "Tiempo CPU secuencial: " << cpu_time_sequential << " segundos\n";

    return 0;
}
