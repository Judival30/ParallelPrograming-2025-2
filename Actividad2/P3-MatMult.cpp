#include <bits/stdc++.h>
#include <pthread.h>
using namespace std;

typedef long long lint;

int N = 6;
int TH = 3;

vector<vector<lint>> A, B, C;

vector<vector<lint>> rnd(int n, int lo = 0, int hi = 999)
{
    random_device rd;
    mt19937 g(rd());
    uniform_int_distribution<int> d(lo, hi);

    vector<vector<lint>> M(n, vector<lint>(n));
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            M[i][j] = d(g);
    return M;
}

void prn(const vector<vector<lint>> &M)
{
    for (auto &r : M)
    {
        for (auto &v : r)
        {
            cout << setw(10) << v << " ";
        }
        cout << "\n";
    }
}

// Multiplicación: cada hilo calcula un rango de columnas
void *mul(void *arg)
{
    long id = (long)arg;
    int blk = (N + TH - 1) / TH;
    int ini = id * blk;
    int fin = min(N, ini + blk);

    for (int j = ini; j < fin; j++)
    {
        for (int i = 0; i < N; i++)
        {
            lint acc = 0;
            for (int k = 0; k < N; k++)
            {
                acc += A[i][k] * B[k][j];
            }
            C[i][j] = acc;
        }
    }
    return nullptr;
}

int main()
{
    N = 6;
    TH = 3;

    A = rnd(N);
    B = rnd(N);
    C.assign(N, vector<lint>(N, 0));

    auto st = chrono::high_resolution_clock::now();

    cout << "Matriz A:\n";
    prn(A);
    cout << "Matriz B:\n";
    prn(B);

    vector<pthread_t> th(TH);
    for (intptr_t t = 0; t < TH; t++)
    {
        pthread_create(&th[t], nullptr, mul, (void *)t);
    }
    for (int t = 0; t < TH; t++)
    {
        pthread_join(th[t], nullptr);
    }

    cout << "Resultado C = A * B:\n";
    prn(C);

    auto en = chrono::high_resolution_clock::now();
    chrono::duration<double> el = en - st;
    cout << "Tiempo de ejecucion: " << el.count() << " segundos\n";

    return 0;
}
