#ifndef TABLERO_H
#define TABLERO_H

#include <bits/stdc++.h>
using namespace std;

class Tablero {
private:
    vector<string> board;
    int N;

public:
    Tablero(int n = 4);
    Tablero(const string &estado, int n = 4);

    void setBoard(const string &estado);
    string toString() const;
    void print() const;

    vector<string> getAcciones() const;
    Tablero mover(const string &accion) const;

    bool esSoluble() const;

    int getSize() const { return N; }


    const vector<string>& getBoard() const { return board; }

    bool operator==(const Tablero &other) const {
        return board == other.board;
    }
};

#endif
