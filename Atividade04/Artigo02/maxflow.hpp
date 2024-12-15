#ifndef MAXFLOW_HPP
#define MAXFLOW_HPP

#include <vector>
#include <algorithm>
#include <queue>
#include <iostream>
#include <limits>
#include <stdexcept>

using namespace std;

template <typename T>
class GraphType
{
public:
    // Atributos principais
    int n;                        // Número de nós
    int m;                        // Número de arestas
    vector<vector<int>> capacity; // Capacidade das arestas
    vector<vector<int>> flow;     // Fluxo nas arestas
    vector<int> excess;           // Excesso de fluxo em cada nó
    vector<int> height;           // Altura de cada nó (técnica Push-Relabel)
    vector<int> count;            // Contagem dos níveis de altura

    int source, sink; // Nó de origem e nó de destino

    // Construtor
    GraphType(int n, int m) : n(n), m(m), capacity(n, vector<int>(n, 0)), flow(n, vector<int>(n, 0)), excess(n, 0), height(n, 0), count(n, 0) {}

    // Adicionar aresta com capacidade
    void add_edge(int u, int v, int cap)
    {
        capacity[u][v] += cap;
    }

    // Adicionar aresta de t-link
    void add_tweights(int v, int sourceCap, int sinkCap)
    {
        add_edge(source, v, sourceCap);
        add_edge(v, sink, sinkCap);
    }

    // Função para executar o algoritmo de max-flow (Push-Relabel)
    T maxflow()
    {
        // Inicializar a altura do nó de origem
        height[source] = n;

        // Inicializar as contagens dos níveis
        count[0] = n - 1;
        count[n] = 1;

        // Inicializa o excesso no nó de origem
        for (int i = 0; i < n; ++i)
        {
            if (capacity[source][i] > 0)
            {
                flow[source][i] = capacity[source][i];
                flow[i][source] = -flow[source][i];
                excess[i] = flow[source][i];
                excess[source] -= flow[source][i];
            }
        }

        // Processamento dos nós com excesso de fluxo
        queue<int> active;
        for (int i = 0; i < n; ++i)
        {
            if (i != source && i != sink && excess[i] > 0)
            {
                active.push(i);
            }
        }

        while (!active.empty())
        {
            int u = active.front();
            active.pop();
            push(u);
        }

        // Retorna o fluxo máximo
        return excess[sink];
    }

    // Função para empurrar fluxo entre dois nós
    void push(int u)
    {
        for (int v = 0; v < n; ++v)
        {
            if (capacity[u][v] > 0 && height[u] == height[v] + 1 && excess[u] > 0)
            {
                int flowToPush = min(excess[u], capacity[u][v] - flow[u][v]);
                flow[u][v] += flowToPush;
                flow[v][u] -= flowToPush;
                excess[u] -= flowToPush;
                excess[v] += flowToPush;

                if (v != source && v != sink && excess[v] == flowToPush)
                {
                    push(v);
                }
                break;
            }
        }
    }

    // Função para verificar o segmento de um nó (se é SOURCE ou SINK)
    int what_segment(int v) const
    {
        return excess[v] > 0 ? source : sink;
    }
};

#endif

/**
 * Análise de complexidade:
 * - A técnica Push-Relabel pode ter uma complexidade na pior caso de O(V^3).
 * - O algoritmo utiliza uma estrutura de fila (queue) para gerenciar os nós ativos e vetores para armazenar capacidades, fluxos, e alturas,
 *   o que torna as operações individuais de push e relabel eficientes.
 */