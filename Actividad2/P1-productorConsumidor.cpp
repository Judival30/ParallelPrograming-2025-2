#include <bits/stdc++.h>
#include <pthread.h>
#include <unistd.h>

using namespace std;

const int N = 10;
const int P = 20;
const int NUM_PROD = 2;
const int NUM_CONS = 2;

int buffer[N];
int cont = 0, in = 0, out = 0;

pthread_mutex_t mut;
pthread_cond_t not_empty, not_full;

void *productor(void *arg)
{
    long id = (long)arg;
    for (int i = 0; i < P; i++)
    {
        pthread_mutex_lock(&mut);
        while (cont == N)
            pthread_cond_wait(&not_full, &mut);

        buffer[in] = i + id * 100;
        printf("Productor %ld produjo: %d\n", id, buffer[in]);
        in = (in + 1) % N;
        cont++;

        pthread_cond_signal(&not_empty);
        pthread_mutex_unlock(&mut);
    }

    pthread_mutex_lock(&mut);
    while (cont == N)
        pthread_cond_wait(&not_full, &mut);

    buffer[in] = -1;
    in = (in + 1) % N;
    cont++;

    pthread_cond_signal(&not_empty);
    pthread_mutex_unlock(&mut);

    return NULL;
}

void *consumidor(void *arg)
{
    long id = (long)arg;
    while (true)
    {
        pthread_mutex_lock(&mut);
        while (cont == 0)
            pthread_cond_wait(&not_empty, &mut);

        int item = buffer[out];
        out = (out + 1) % N;
        cont--;

        pthread_cond_signal(&not_full);
        pthread_mutex_unlock(&mut);

        if (item == -1)
        {
            printf("Consumidor %ld recibió sentinela, termina.\n", id);
            break;
        }

        printf("Consumidor %ld consumió: %d\n", id, item);
    }
    return NULL;
}

int main()
{
    pthread_mutex_init(&mut, NULL);
    pthread_cond_init(&not_empty, NULL);
    pthread_cond_init(&not_full, NULL);

    vector<pthread_t> productores(NUM_PROD), consumidores(NUM_CONS);

    for (long i = 0; i < NUM_PROD; i++)
        pthread_create(&productores[i], NULL, productor, (void *)i);

    for (long i = 0; i < NUM_CONS; i++)
        pthread_create(&consumidores[i], NULL, consumidor, (void *)i);

    for (auto &t : productores)
        pthread_join(t, NULL);

    for (auto &t : consumidores)
        pthread_join(t, NULL);

    pthread_mutex_destroy(&mut);
    pthread_cond_destroy(&not_empty);
    pthread_cond_destroy(&not_full);
    return 0;
}
