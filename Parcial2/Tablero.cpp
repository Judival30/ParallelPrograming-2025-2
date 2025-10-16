#include "Tablero.h"

Tablero::Tablero(int n) : N(n), board(n, string(n, '#')) {}

Tablero::Tablero(const string &estado, int n) : N(n) {
    setBoard(estado);
}

void Tablero::setBoard(const string &estado) {
    board.clear();
    for (int i = 0; i < N; i++)
        board.push_back(estado.substr(i * N, N));
}

string Tablero::toString() const {
    string s;
    for (auto &r : board) s += r;
    return s;
}

void Tablero::print() const {
    for (auto &row : board) {
        for (auto &ch : row) cout << ch << " ";
        cout << "\n";
    }
    cout << "\n";
}

vector<string> Tablero::getAcciones() const {
    vector<string> acts;
    int r=-1, c=-1;
    for (int i=0;i<N;i++)
        for (int j=0;j<N;j++)
            if (board[i][j]=='#'){r=i;c=j;}

    if (r>0) acts.push_back("UP");
    if (r<N-1) acts.push_back("DOWN");
    if (c>0) acts.push_back("LEFT");
    if (c<N-1) acts.push_back("RIGHT");
    return acts;
}

Tablero Tablero::mover(const string &accion) const {
    vector<string> nb = board;
    int r=-1,c=-1;
    for(int i=0;i<N;i++)
        for(int j=0;j<N;j++)
            if(board[i][j]=='#'){r=i;c=j;}

    int nr=r,nc=c;
    if(accion=="UP") nr--;
    else if(accion=="DOWN") nr++;
    else if(accion=="LEFT") nc--;
    else if(accion=="RIGHT") nc++;

    if(nr<0||nr>=N||nc<0||nc>=N) return *this;
    swap(nb[r][c],nb[nr][nc]);
    Tablero nuevo(nb[0]+nb[1]+nb[2]+nb[3], N);
    nuevo.setBoard(nb[0]+nb[1]+nb[2]+nb[3]);
    return nuevo;
}


