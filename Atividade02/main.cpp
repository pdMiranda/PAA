#include <iostream>
#include "tree_edit.h"

int main() {
    // Geração totalmente randômica (padrão)
    Node* T1 = generateRandomTree(6);
    Node* T2 = generateRandomTree(8);

    // Ou, se quiser customizar:
    // std::vector<std::string> labels = {"A", "B", "C", "D", "X", "Y"};
    // Node* T1 = generateRandomTree(3, 3, labels, minNodes);
    // Node* T2 = generateRandomTree(3, 3, labels, minNodes);

    std::cout << "Iniciando cálculo da distância de edição entre as árvores T1 e T2...\n \n";
    int result = treeEditDistance(T1, T2);

    std::cout << "\nResultado final: Distância de edição entre T1 e T2 = " << result << "\n";
    return 0;
}