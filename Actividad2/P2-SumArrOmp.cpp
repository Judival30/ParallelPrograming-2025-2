#include <bits/stdc++.h>
#include <omp.h>

using namespace std;
typedef long long lint;

vector<lint> genArr(lint N)
{
    vector<lint> A(N);
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<lint> dist(1, 100);

    for (lint i = 0; i < N; i++)
        A[i] = dist(gen);

    return A;
}

lint parSum(const vector<lint> &A, double &t)
{
    lint ans = 0;
    double ini = omp_get_wtime();

#pragma omp parallel for reduction(+ : ans)
    for (lint i = 0; i < (lint)A.size(); i++)
        ans += A[i];

    double fin = omp_get_wtime();
    t = fin - ini;
    return ans;
}

int main()
{
    lint N = 1e7;

    // Generar arreglo
    vector<lint> A = genArr(N);

    // Ejecutar suma paralela
    double tp;
    lint ansPar = parSum(A, tp);

    cout << "Suma paralela: " << ansPar << endl;
    cout << "Tiempo paralelo: " << tp << " segundos\n";

    return 0;
}
