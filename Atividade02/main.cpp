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
    std::cout << "Complexidade teórica: O(n1·n2·D²), onde n1 e n2 são os tamanhos das árvores e D é o grau máximo\n\n";

    Node* T1 = generateRandomTree(6);
    Node* T2 = generateRandomTree(8);

    // std::vector<std::string> labels = {"A", "B", "C", "D", "X", "Y"};
    // Node* T1 = generateRandomTree(3, 3, labels, minNodes);
    // Node* T2 = generateRandomTree(3, 3, labels, minNodes);

    std::cout << "Iniciando cálculo da distância de edição entre as árvores T1 e T2...\n \n";
    int result = treeEditDistance(T1, T2);

    //LOGS
    //int result = treeEditDistance(T1, T2, true);

    int n1 = countNodes(T1);
    int n2 = countNodes(T2);
    int d1 = maxDegree(T1);
    int d2 = maxDegree(T2);
    int D = std::max(d1, d2);

    std::cout << "\nComplexidade teórica nesta execução: O(" << n1 << "·" << n2 << "·" << D << "²)\n";
    std::cout << "\nResultado final: Distância de edição entre T1 e T2 = " << result << "\n";
    return 0;
}