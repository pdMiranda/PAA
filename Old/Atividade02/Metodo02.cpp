#include <iostream>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <ctime>

using namespace std;

int find(int* array, int X, int ops){
    int achou = 0; // 2 operações
    int i = 0; // 2 operações
 
    while(achou == 0){ // 1 operação
        if(array[i] == X){  // 1 operação
            achou = 1; // 1 operação
        }
        i++; // 1 operação

        ops += 4;
    }

    return ops;  // 1 operação
}

int main() {
    int v[] = {4, 3, 1, 8, 2, 7, 5}; 
    int x = 2;

    int ops = 9; //operações iniciais
    int result = find(v, x, ops);

    cout << "Operações: " << result << endl;
    
}