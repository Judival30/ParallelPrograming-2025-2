#ifndef ALGORITMOS_H
#define ALGORITMOS_H

#include "Tablero.h"
#include <omp.h>

int BFS(const Tablero &inicio);
int BFS_parallel(const Tablero &inicio, int n_hilos);
int AStar_h1(const Tablero &inicio);
int AStar_h2(const Tablero &inicio);

int h1(const Tablero &t);
int h2(const Tablero &t);

void compararParalelo(const Tablero &inicio);
int AStar_h2_dataDecomposition(const Tablero &ini, int n_hilos);



#endif
