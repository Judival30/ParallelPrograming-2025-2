#include <iostream>
#include <omp.h>
#include "Tablero.h"
#include "Algoritmos.h"

using namespace std;    

void mostrarMenu() {
    cout << "\n==============================\n";
    cout << "           OPCIONES           \n";
    cout << "==============================\n";
    cout << "1. BFS secuencial\n";
    cout << "2. BFS paralelo\n";
    cout << "3. A* con heurística h1\n";
    cout << "4. A* con heurística h2\n";
    cout << "5. A* paralelo (descomposición de datos)\n";
    cout << "6. [Tarea 1] Mostrar tablero 4x4 desde cadena\n";
    cout << "7. [Tarea 2] Ejecutar movimiento (UP/DOWN/LEFT/RIGHT)\n";
    cout << "8. Comparar BFS, A*-h1 y A*-h2 en paralelo\n";
    cout << "9. Salir\n";
    cout << "Seleccione una opción: ";
}

int main() {
    string estado;
    int opcion;
    int n = 4;

    cout << "Ingrese el estado inicial del tablero (ejemplo: LAIB#KGCDOHENMJF): ";
    cin >> estado;

    Tablero t(estado, n);
    cout << "\nTablero inicial:\n";
    t.print();

    do {
        mostrarMenu();
        cin >> opcion;
        cout << "\n";

        double ini = 0, fin = 0;
        int ans = -1;

        switch (opcion) {
            case 1:
                cout << "Ejecutando BFS secuencial...\n";
                ini = omp_get_wtime();
                ans = BFS(t);
                fin = omp_get_wtime();
                cout << "Resultado BFS: " << ans << " movimientos.\n";
                cout << "Tiempo: " << (fin - ini) << " s\n";
                break;

            case 2: {
                int hilos;
                cout << "Ingrese número de hilos: ";
                cin >> hilos;
                cout << "Ejecutando BFS paralelo...\n";
                ini = omp_get_wtime();
                ans = BFS_parallel(t, hilos);
                fin = omp_get_wtime();
                cout << "Resultado BFS paralelo: " << ans << " movimientos.\n";
                cout << "Tiempo: " << (fin - ini) << " s\n";
                break;
            }

            case 3:
                cout << "Ejecutando A* con heurística h1...\n";
                ini = omp_get_wtime();
                ans = AStar_h1(t);
                fin = omp_get_wtime();
                cout << "Resultado A*-h1: " << ans << " movimientos.\n";
                cout << "Tiempo: " << (fin - ini) << " s\n";
                break;

            case 4:
                cout << "Ejecutando A* con heurística h2...\n";
                ini = omp_get_wtime();
                ans = AStar_h2(t);
                fin = omp_get_wtime();
                cout << "Resultado A*-h2: " << ans << " movimientos.\n";
                cout << "Tiempo: " << (fin - ini) << " s\n";
                break;

            case 5: {
                int hilos;
                cout << "Ingrese número de hilos: ";
                cin >> hilos;
                cout << "Ejecutando A* paralelo (descomposición de datos)...\n";
                ini = omp_get_wtime();
                ans = AStar_h2_dataDecomposition(t, hilos);
                fin = omp_get_wtime();
                cout << "Resultado A* paralelo: " << ans << " movimientos.\n";
                cout << "Tiempo: " << (fin - ini) << " s\n";
                break;
            }

            case 6: {
                string entrada;
                cout << "Ingrese la cadena de 16 caracteres: ";
                cin >> entrada;
                Tablero tablero(entrada, 4);
                cout << "Tablero generado:\n";
                tablero.print();
                break;
            }

            case 7: {
                string entrada, accion;
                cout << "Ingrese la cadena de 16 caracteres: ";
                cin >> entrada;
                cout << "Ingrese la acción (UP, DOWN, LEFT, RIGHT): ";
                cin >> accion;
                Tablero tablero(entrada, 4);
                tablero.mover(accion);
                cout << "Tablero resultante:\n";
                tablero.print();
                break;
            }

            case 8:
                cout << "Comparando BFS, A*-h1 y A*-h2 en paralelo...\n";
                compararParalelo(t);
                break;

            case 9:
                cout << "👋 Saliendo del programa.\n";
                break;

            default:
                cout << "Opción no válida.\n";
        }

    } while (opcion != 9);

    return 0;
}
