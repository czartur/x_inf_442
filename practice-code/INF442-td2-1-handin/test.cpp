#include <iostream>
#include <algorithm>
using namespace std;

int main(){
    int a[4] = {2, 1, 4, 3};
    sort(a, a+4);
    for(int i=0; i<4; i++) cout << a[i] << " ";
    cout << endl;
} 