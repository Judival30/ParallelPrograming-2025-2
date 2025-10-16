#include <iostream>
#include <omp.h>
#include "Tablero.h"
#include "Algoritmos.h"

using namespace std;    

int main() {
    string estado;
    while(cin >> estado){
        Tablero t(estado, 4);
        double ini = omp_get_wtime();
        int ans = AStar_h2(t);
        double fin = omp_get_wtime();
        cout << fin - ini << "\n";
    }
    return 0;
}
