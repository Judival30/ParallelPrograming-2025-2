#include "Algoritmos.h"

int h1(const Tablero &t) {
    auto b = t.getBoard();
    int N = t.getSize();
    int h = 0;
    char target = 'A';
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            if (b[i][j] == '#') continue;
            if (b[i][j] != target) h++;
            target++;
        }
    return h;
}

pair<int,int> posObjetivo(char ch, int N) {
    if (ch == '#') return {N - 1, N - 1};
    int idx = ch - 'A';
    return {idx / N, idx % N};
}

int h2(const Tablero &t) {
    auto b = t.getBoard();
    int N = t.getSize();
    int sum = 0;
    int gr, gc;
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            char ch = b[i][j];
            if (ch == '#') continue;
            pair<int, int> tmp = posObjetivo(ch, N);
            gr = tmp.first;
            gc = tmp.second;
            sum += abs(gr - i) + abs(gc - j);
        }
    return sum;
}

int BFS(const Tablero &ini) {
    

    int N = ini.getSize(), dist;
    string goal;
    for (char c = 'A'; goal.size() < N * N - 1; c++) goal.push_back(c);
    goal.push_back('#');

    queue<pair<string,int>> q;
    unordered_set<string> vis;
    string start = ini.toString();

    q.push({start, 0});
    vis.insert(start);
    string estado;

    long long nodosExpandidos = 0;
    while (!q.empty()) {
        estado = q.front().first;
        dist = q.front().second;
        q.pop();
        nodosExpandidos++;
        if (estado == goal){ 
            cout << nodosExpandidos << " ";
            return dist;
        }

        Tablero t(estado, N);
        for (auto &act : t.getAcciones()) {
            Tablero nt = t.mover(act);
            string ns = nt.toString();
            if (!vis.count(ns)) {
                vis.insert(ns);
                q.push({ns, dist + 1});
            }
        }
    }
    return -1;
}
int BFS_parallel(const Tablero &ini, int n_hilos = 4) {
    

    int N = ini.getSize();
    string goal;
    for (char c = 'A'; goal.size() < N * N - 1; c++) goal.push_back(c);
    goal.push_back('#');

    string start = ini.toString();
    unordered_set<string> visitados;
    visitados.insert(start);

    queue<pair<string, int>> q;
    q.push(make_pair(start, 0));

    int dist = -1;
    bool found = false;

    double start_time = omp_get_wtime();

    while (!q.empty() && !found) {
        int nivel_size = q.size();
        vector<pair<string, int>> nivel_actual;

        // Extraer el nivel actual
        for (int i = 0; i < nivel_size; ++i) {
            nivel_actual.push_back(q.front());
            q.pop();
        }

        vector<pair<string, int>> nuevos;

        #pragma omp parallel for num_threads(n_hilos) schedule(dynamic)
        for (int i = 0; i < (int)nivel_actual.size(); ++i) {
            string estado = nivel_actual[i].first;
            int d = nivel_actual[i].second;

            if (estado == goal) {
                #pragma omp critical
                { found = true; dist = d; }
                continue;
            }

            Tablero t(estado, N);
            vector<string> acts = t.getAcciones();

            for (int j = 0; j < (int)acts.size(); ++j) {
                Tablero nt = t.mover(acts[j]);
                string ns = nt.toString();

                bool ya_visitado = false;
                #pragma omp critical(vis)
                {
                    if (visitados.count(ns)) ya_visitado = true;
                    else visitados.insert(ns);
                }

                if (!ya_visitado) {
                    #pragma omp critical(queue)
                    nuevos.push_back(make_pair(ns, d + 1));
                }
            }
        }
        for (int i = 0; i < (int)nuevos.size(); ++i)
            q.push(nuevos[i]);
    }

    return dist;
}


int AStar_h1(const Tablero &ini) {
    

    int N = ini.getSize();
    string goal;
    for (char c = 'A'; goal.size() < N * N - 1; c++) goal.push_back(c);
    goal.push_back('#');

    struct Node { string s; int g, f; };
    auto cmp = [](Node a, Node b){ return a.f > b.f; };
    priority_queue<Node, vector<Node>, decltype(cmp)> pq(cmp);
    unordered_set<string> vis;

    pq.push({ini.toString(), 0, h1(ini)});

    long long nodosExpandidos = 0;
    while (!pq.empty()) {
        Node cur = pq.top(); pq.pop();
        nodosExpandidos++;
        if (cur.s == goal){
            cout << nodosExpandidos << " ";
            return cur.g;
        }
        if (vis.count(cur.s)) continue;
        vis.insert(cur.s);

        Tablero t(cur.s, N);
        for (auto &act : t.getAcciones()) {
            Tablero nt = t.mover(act);
            string ns = nt.toString();
            if (!vis.count(ns)) {
                int g2 = cur.g + 1;
                int f2 = g2 + h1(nt);
                pq.push({ns, g2, f2});
            }
        }
    }
    return -1;
}

int AStar_h2(const Tablero &ini) {
    int N = ini.getSize();
    string goal;
    for (char c = 'A'; goal.size() < N * N - 1; c++) goal.push_back(c);
    goal.push_back('#');

    struct Node { string s; int g, f; };
    auto cmp = [](Node a, Node b){ return a.f > b.f; };
    priority_queue<Node, vector<Node>, decltype(cmp)> pq(cmp);
    unordered_set<string> vis;
    long long nodosExpandidos = 0;
    pq.push({ini.toString(), 0, h2(ini)});

    while (!pq.empty()) {
        Node cur = pq.top(); pq.pop();
        nodosExpandidos++;
        if (cur.s == goal){
            cout << nodosExpandidos << " ";
            return cur.g;
        }
        if (vis.count(cur.s)) continue;
        vis.insert(cur.s);

        Tablero t(cur.s, N);
        for (auto &act : t.getAcciones()) {
            Tablero nt = t.mover(act);
            string ns = nt.toString();
            if (!vis.count(ns)) {
                int g2 = cur.g + 1;
                int f2 = g2 + h2(nt);
                pq.push({ns, g2, f2});
            }
        }
    }
    return -1;
}

void compararParalelo(const Tablero &ini) {
    int bfs_c, a1_c, a2_c;
    double t1 = omp_get_wtime();

    #pragma omp parallel sections
    {
        #pragma omp section
        { bfs_c = BFS(ini); }

        #pragma omp section
        { a1_c = AStar_h1(ini); }

        #pragma omp section
        { a2_c = AStar_h2(ini); }
    }

    double t2 = omp_get_wtime();
    cout << "\nBFS: " << bfs_c
         << "\nA*-h1: " << a1_c
         << "\nA*-h2: " << a2_c
         << "\nTiempo total paralelo: " << (t2 - t1) << " s\n";
}

int AStar_h2_dataDecomposition(const Tablero &ini, int n_hilos = 4) {
   

    int N = ini.getSize();
    string goal;
    for (char c = 'A'; goal.size() < N*N - 1; c++) goal.push_back(c);
    goal.push_back('#');

    struct Node { string s; int g, f; };
    auto cmp = [](Node a, Node b){ return a.f > b.f; };

    string start = ini.toString();
    unordered_set<string> visited_global;
    visited_global.insert(start);

    vector<Node> iniciales;
    Tablero t0 = ini;
    for (auto &act : t0.getAcciones()) {
        Tablero nt = t0.mover(act);
        string ns = nt.toString();
        iniciales.push_back({ns, 1, 1 + h2(nt)});
        visited_global.insert(ns);
    }

    atomic<int> mejorSol(-1);
    double t_ini = omp_get_wtime();

  
    #pragma omp parallel num_threads(n_hilos)
    {
        priority_queue<Node, vector<Node>, decltype(cmp)> pq(cmp);
        unordered_set<string> local_vis;
        int id = omp_get_thread_num();


        #pragma omp for schedule(static)
        for (int i = 0; i < (int)iniciales.size(); ++i)
            pq.push(iniciales[i]);

        while (!pq.empty() && (mejorSol == -1)) {
            Node cur = pq.top(); pq.pop();
            if (cur.s == goal) {
                #pragma omp critical
                {
                    if (mejorSol == -1 || cur.g < mejorSol)
                        mejorSol = cur.g;
                }
                break;
            }

            if (local_vis.count(cur.s)) continue;
            local_vis.insert(cur.s);

            Tablero t(cur.s, N);
            for (auto &act : t.getAcciones()) {
                Tablero nt = t.mover(act);
                string ns = nt.toString();

                bool skip = false;
                #pragma omp critical(visited)
                {
                    if (visited_global.count(ns))
                        skip = true;
                    else
                        visited_global.insert(ns);
                }

                if (!skip) {
                    int g2 = cur.g + 1;
                    int f2 = g2 + h2(nt);
                    pq.push({ns, g2, f2});
                }
            }
        }
    }

    double t_fin = omp_get_wtime();
    return mejorSol;
}

