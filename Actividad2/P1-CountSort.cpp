#include <bits/stdc++.h>
#include <pthread.h>
using namespace std;

const int N = 10;
const int THREADS = 2;

vector<int> A = {15, 3, 9, 20, 1, 7, 12, 18, 5, 2};

vector<int> B(N);
int thread_count = THREADS;

void *count_sort(void *rank)
{
    long my_rank = (long)rank;
    int n = N / thread_count;
    int ini = my_rank * n;
    int end = (my_rank + 1) * n;

    for (int i = ini; i < end; i++)
    {
        int count = 0;
        for (int j = 0; j < N; j++)
        {
            if (A[j] < A[i] || (A[j] == A[i] && j < i))
                count++;
        }
        B[count] = A[i];
    }
    return nullptr;
}

int main()
{
    pthread_t threads[THREADS];

    for (long t = 0; t < THREADS; t++)
        pthread_create(&threads[t], nullptr, count_sort, (void *)t);

    for (int t = 0; t < THREADS; t++)
        pthread_join(threads[t], nullptr);

    cout << "Arreglo ordenado original:\n";
    for (int i = 0; i < N; i++)
        cout << A[i] << " ";
    cout << "\n";
    cout << "Arreglo ordenado (Count Sort paralelo):" << endl;
    for (int i = 0; i < N; i++)
        cout << B[i] << " ";
    cout << "\n";

    return 0;
}
