#include <bits/stdc++.h>
#include <omp.h>

using lint = long long;
using namespace std;

int main(){

    vector<vector<lint>> A(10, vector<lint>(100000, 43));
    vector<lint> ans(A.size(), 0);

    #pragma omp parallel for
    for (int i = 0; i < (int)A.size(); ++i){
        lint s = 0;
        for (lint v : A[i]) s += v;
        ans[i] = s;
    }
    cout << "ans:\n";
    for (lint x : ans) cout << x << " ";
    cout << "\n";
    return 0;
}
