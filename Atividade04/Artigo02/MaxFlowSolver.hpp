#ifndef MAXFLOWSOLVER_HPP
#define MAXFLOWSOLVER_HPP

#include <vector>
#include <limits>
#include <iostream>
#include <stdexcept>
#include "graph.hpp" // Certifique-se de incluir o arquivo da estrutura de grafo

using namespace std;

class MaxFlowSolver
{
public:
    MaxFlowSolver(int n) : n(n)
    {
        try
        {
            // Inicializando as matrizes de capacidade, fluxo e capacidade residual
            capacity.resize(n, vector<double>(n, 0.0));
            flow.resize(n, vector<double>(n, 0.0));
            residualCapacity.resize(n, vector<double>(n, 0.0));
        }
        catch (const std::bad_alloc &e)
        {
            cerr << "Erro de alocacao de memoria: " << e.what() << endl;
            throw; // Re-lançar a exceção para que o programa possa tratar isso em outro nível
        }
    }

    void addEdge(int u, int v, double cap)
    {
        capacity[u][v] = cap;
        residualCapacity[u][v] = cap;
    }

    double edmondsKarp(int source, int sink)
    {
        double maxFlow = 0;

        while (true)
        {
            vector<int> parent(n, -1);
            vector<bool> visited(n, false);

            if (!bfs(source, sink, parent, visited))
                break; // Se não encontrar um caminho aumentante, termina

            // Encontrar o fluxo mínimo ao longo do caminho aumentante
            double pathFlow = numeric_limits<double>::infinity();
            int s = sink;
            while (s != source)
            {
                int p = parent[s];
                pathFlow = min(pathFlow, residualCapacity[p][s]);
                s = p;
            }

            // Atualizar a capacidade residual
            s = sink;
            while (s != source)
            {
                int p = parent[s];
                residualCapacity[p][s] -= pathFlow;
                residualCapacity[s][p] += pathFlow;
                s = p;
            }

            maxFlow += pathFlow;
        }

        return maxFlow;
    }

private:
    int n; // Número de vértices no grafo
    vector<vector<double>> capacity;
    vector<vector<double>> flow;
    vector<vector<double>> residualCapacity;

    bool bfs(int source, int sink, vector<int> &parent, vector<bool> &visited)
    {
        visited[source] = true;
        vector<int> queue = {source};

        while (!queue.empty())
        {
            int u = queue.back();
            queue.pop_back();

            for (int v = 0; v < n; ++v)
            {
                if (!visited[v] && residualCapacity[u][v] > 0)
                {
                    parent[v] = u;
                    visited[v] = true;

                    if (v == sink)
                        return true;

                    queue.push_back(v);
                }
            }
        }

        return false;
    }
};

#endif
