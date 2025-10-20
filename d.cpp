#include <bits/stdc++.h>
#include <omp.h>


using namespace std;

int main(){

    cout<<setprecision(6) << fixed;

    double g = 9.81;  
    vector<double> A = { 0.01,0.5,1.8,5.0, 12.5,45.0,100.0, 500.0, 8848.0, 39000.0,384400000.0};

    vector<double> ans(A.size());

    #pragma omp parallel for
    for (int i = 0; i < (int)A.size(); ++i) ans[i] = sqrt(2.0*A[i]/g);
    
    for (int i = 0; i < (int)A.size(); ++i){
        cout << "h=" << A[i] << "  t=" << ans[i] << " s\n";
    }
    return 0;
}
