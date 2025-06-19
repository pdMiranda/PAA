#include <iostream>
#include "tree_edit.h"

int countNodes(Node* n) {
    if (!n) return 0;
    int cnt = 1;
    for (auto* c : n->children) cnt += countNodes(c);
    return cnt;
}

int maxDegree(Node* n) {
    if (!n) return 0;
    int maxD = n->children.size();
    for (auto* c : n->children) {
        int childD = maxDegree(c);
        if (childD > maxD) maxD = childD;
    }
    return maxD;
}

int main() {

    std::cout << "Algoritmo: Zhang-Shasha (Tree Edit Distance)\n";
    std::cout << "Complexidade teorica: O(n1·n2·D²), onde n1 e n2 sao os tamanhos das arvores e D eh o grau maximo\n\n";

    Node* T1 = generateRandomTree(6);
    Node* T2 = generateRandomTree(8);

    // std::vector<std::string> labels = {"A", "B", "C", "D", "X", "Y"};
    // Node* T1 = generateRandomTree(3, 3, labels, minNodes);
    // Node* T2 = generateRandomTree(3, 3, labels, minNodes);

    std::cout << "Iniciando calculo da distancia de edicao entre as arvores T1 e T2...\n \n";
    int result = treeEditDistance(T1, T2);

    //LOGS
    //int result = treeEditDistance(T1, T2, true);

    int n1 = countNodes(T1);
    int n2 = countNodes(T2);
    int d1 = maxDegree(T1);
    int d2 = maxDegree(T2);
    int D = std::max(d1, d2);

    std::cout << "\nComplexidade teorica nesta execucao: O(" << n1 << "·" << n2 << "·" << D << "²) -> O(n1·n2·D²)\n";
    std::cout << "\nResultado final: Distancia de edicao entre T1 e T2 = " << result << "\n";
    return 0;
}