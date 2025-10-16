#include <iostream>
#include <omp.h>
#include "Tablero.h"
#include "Algoritmos.h"
#include <cmath>  

using namespace std;    

int main() {
    string estado;
    while(cin >> estado){
        int n = sqrt((int)estado.size());
        Tablero t(estado, n);
        double ini = omp_get_wtime();
        int ans = AStar_h2(t);
        double fin = omp_get_wtime();
        cout << fin - ini << "\n";
    }
    return 0;
}
