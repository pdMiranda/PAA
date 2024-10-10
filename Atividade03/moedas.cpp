#include <iostream>
#include <vector>
#include <climits>
#include <map>     
using namespace std;

// Algoritmo guloso
pair<int, map<int, int>> greedyChange(vector<int> &coins, int value) {
    map<int, int> coinCountMap; 
    int coinCount = 0;
    
    for (int i = coins.size() - 1; i >= 0; i--) {
        while (value >= coins[i]) {
            value -= coins[i];
            coinCount++;
            coinCountMap[coins[i]]++; 
        }
    }

    return {coinCount, coinCountMap};
}

// Auxiliar algoritmo combinações
void findMinCoins(vector<int> &coins, int index, int value, int currentCoins, int &minCoins, map<int, int> &coinCountMap, map<int, int> &bestCoinCountMap) {
    if (value == 0) {
        if (currentCoins < minCoins) {
            minCoins = currentCoins;
            bestCoinCountMap = coinCountMap;
        }
        return;
    }

    if (value < 0 || index >= coins.size()) {
        return;
    }

    // Tentativa 1: Incluir a moeda atual e continuar com o valor reduzido
    coinCountMap[coins[index]]++;
    findMinCoins(coins, index, value - coins[index], currentCoins + 1, minCoins, coinCountMap, bestCoinCountMap);
    coinCountMap[coins[index]]--;

    // Tentativa 2: Não incluir a moeda atual e passar para a proxima moeda
    findMinCoins(coins, index + 1, value, currentCoins, minCoins, coinCountMap, bestCoinCountMap);
}

// Encontrar minimo de moedas com todas as combinações
pair<int, map<int, int>> exhaustiveChange(vector<int> &coins, int value) {
    int minCoins = INT_MAX;
    map<int, int> coinCountMap;
    map<int, int> bestCoinCountMap;

    findMinCoins(coins, 0, value, 0, minCoins, coinCountMap, bestCoinCountMap);
    
    return {minCoins, bestCoinCountMap};
}


void printCoinChoices(const map<int, int> &coinCountMap) {
    for (const auto &entry : coinCountMap) {
        cout << "Moeda " << entry.first << ": usada " << entry.second << " vez(es)" << endl;
    }
}


void analyzeOptimalityConditions(vector<vector<int>> coinSets, vector<int> values) {
    for (auto &coins : coinSets) { 
        for (int value : values) {
            cout << "\n\n--- Analisando Conjunto de Moedas: {";
            for (size_t i = 0; i < coins.size(); i++) {
                cout << coins[i];
                if (i != coins.size() - 1) cout << ", ";
            }
            cout << "} e Valor Alvo: " << value << " ---\n";

            // Algoritmo Guloso
            auto greedyResult = greedyChange(coins, value);
            cout << "\nAlgoritmo Guloso - Menor numero de moedas: " << greedyResult.first << endl;
            cout << "Moedas escolhidas pelo Algoritmo Guloso:\n";
            printCoinChoices(greedyResult.second);

            // Algoritmo de Todas as Combinações
            auto exhaustiveResult = exhaustiveChange(coins, value);
            cout << "\nAlgoritmo de Todas as Combinacoes - Menor numero de moedas: " << exhaustiveResult.first << endl;
            cout << "Moedas escolhidas pelo Algoritmo de Todas as Combinacoes:\n";
            printCoinChoices(exhaustiveResult.second);

            // Analisando falhas do algoritmo guloso
            if (greedyResult.first != exhaustiveResult.first) {
                cout << "\nO algoritmo guloso falhou em encontrar a solucao otima." << endl;
            } else {
                cout << "\nO algoritmo guloso encontrou a solucao otima." << endl;
            }
        }
    }
}

int main() {
    vector<vector<int>> coinSets = {
        {1, 3, 4},  
        {1, 2, 5},  
        {1, 5, 10},  
        {1, 6, 10} 
    };   

    vector<int> values = {6, 8, 10, 12}; 

    analyzeOptimalityConditions(coinSets, values);

    return 0;
}
