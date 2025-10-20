#include <bits/stdc++.h>
#include <omp.h>
using namespace std;

vector<vector<int>> L = {
    {1, -5, 2, 3, 9999},        
    {},                       
    {42, 42, 42, 42, 42, 42},  
    {7, 1, -3, 7, 2, -3, 7},   
    {1000000},               
    {9, 8, 7, 6, 5, 4, 3, 2, 1},
    {1, 2, 3, 4, 5, 6, 7, 8, 9},
    {-1, -2, -3, -4},          
    {0, 0, 0, 1, 0, 0, 2},       
    {13, 17, 19, 23, 29, 31}    
};


int main(){
    
    int x = 2;
    vector<bool> ans(L.size(), 0); 

    #pragma omp parallel for
    for (int i = 0; i < (int)L.size(); ++i){
        bool flag = false;
        for (int v : L[i]){
            if (v == x){ flag = true; break; }
        }
        ans[i] = flag;
    }

    cout << "Ans:\n";
    for (bool v : ans) cout << v << " ";
    cout << "\n";
    return 0;
}
