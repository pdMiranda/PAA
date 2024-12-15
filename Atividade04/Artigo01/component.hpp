#ifndef COMPONENT_H
#define COMPONENT_H

#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>
#include "graph.hpp"

using namespace std;

class GraphComponents
{
public:
    vector<int> parent; // Vetor para armazenar o pai de cada vértice
    vector<int> rank;   // Vetor para armazenar a "altura" da árvore de cada conjunto

    // Construtor para inicializar os conjuntos disjuntos
    GraphComponents(int numVertices)
    {
        parent.resize(numVertices);
        rank.resize(numVertices, 0);
        for (int i = 0; i < numVertices; ++i)
        {
            parent[i] = i; // Cada vértice é seu próprio pai inicialmente
        }
    }

    // Encontrar o representante (raiz) do conjunto ao qual o vértice pertence
    int find(int vertex)
    {
        if (parent[vertex] != vertex)
        {
            parent[vertex] = find(parent[vertex]); // Compressão de caminho
        }
        return parent[vertex];
    }

    // Unir dois conjuntos com base na classificação (rank)
    void unite(int vertex1, int vertex2)
    {
        int root1 = find(vertex1);
        int root2 = find(vertex2);

        if (root1 != root2)
        {
            if (rank[root1] > rank[root2])
            {
                parent[root2] = root1;
            }
            else if (rank[root1] < rank[root2])
            {
                parent[root1] = root2;
            }
            else
            {
                parent[root2] = root1;
                rank[root1]++;
            }
        }
    }

    // Calcular a diferença interna da componente (Int(C))
    double calculateInternalDifference(const vector<Edge> &mstEdges)
    {
        double maxWeight = 0.0;
        for (const auto &edge : mstEdges)
        {
            maxWeight = max(maxWeight, edge.weightR);
        }
        return maxWeight; // Int(C): maior peso na MST da componente
    }

    // Implementar o algoritmo de segmentação com base no artigo
    void segmentGraph(Graph &graph, double k)
    {
        vector<double> internalDifference(graph.numVertices, 0.0); // Int(C)
        vector<int> componentSize(graph.numVertices, 1);           // |C|

        // Ordenar as arestas por peso
        sort(graph.edges.begin(), graph.edges.end(), [](const Edge &a, const Edge &b)
             { return a.weightR < b.weightR; });

        // Processar arestas para segmentação
        for (const auto &edge : graph.edges)
        {
            int root1 = find(edge.src);
            int root2 = find(edge.dest);

            if (root1 != root2)
            {
                double diff = edge.weightR;             // Dif(C1, C2): menor peso entre componentes
                double tau1 = k / componentSize[root1]; // k/|C1|
                double tau2 = k / componentSize[root2]; // k/|C2|

                double minInternalDifference = min(internalDifference[root1] + tau1, internalDifference[root2] + tau2); // MInt(C1, C2)

                // Verificar o predicado D(C1, C2)
                if (diff <= minInternalDifference)
                {
                    // Unir os componentes
                    unite(root1, root2);
                    int newRoot = find(root1);

                    // Atualizar Int(C) e tamanho da nova componente
                    internalDifference[newRoot] = max({internalDifference[root1], internalDifference[root2], diff});
                    componentSize[newRoot] = componentSize[root1] + componentSize[root2];
                }
            }
        }
    }

    // Obter o número de componentes conexas
    int getNumberOfComponents()
    {
        int components = 0;
        for (int i = 0; i < parent.size(); ++i)
        {
            if (parent[i] == i)
            {
                components++;
            }
        }
        return components;
    }

    // Exibir as componentes conexas
    void printComponents()
    {
        vector<vector<int>> components(parent.size());
        for (int i = 0; i < parent.size(); ++i)
        {
            components[find(i)].push_back(i);
        }

        for (const auto &component : components)
        {
            if (!component.empty())
            {
                cout << "Componente: ";
                for (int vertex : component)
                {
                    cout << vertex << " ";
                }
                cout << endl;
            }
        }
    }
};

#endif