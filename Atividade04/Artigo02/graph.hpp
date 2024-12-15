#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <queue>
#include <limits>
#include "imageloader.hpp"

using namespace std;

// Estrutura para uma aresta do grafo
struct Edge
{
    int src;       // Vértice de origem
    int dest;      // Vértice de destino
    double weight; // Peso da aresta
};

// Classe para representar o grafo
class Graph
{
public:
    int numVertices;                 // Número total de vértices (pixels)
    vector<Edge> edges;              // Lista de arestas
    vector<vector<double>> capacity; // Matriz de capacidade
    vector<vector<int>> adj;         // Lista de adjacências
    vector<double> tLinkSource;      // Pesos para os t-links conectando ao source
    vector<double> tLinkSink;        // Pesos para os t-links conectando ao sink
    int source;                      // Nó source
    int sink;                        // Nó sink

    // Construtor
    Graph(const vector<vector<Pixel>> &image, int width, int height, double sigma, const vector<double> &objProb, const vector<double> &bgProb)
    {
        numVertices = width * height + 2; // +2 para source e sink
        source = numVertices - 2;
        sink = numVertices - 1;

        capacity.resize(numVertices, vector<double>(numVertices, 0.0));
        adj.resize(numVertices);

        tLinkSource.resize(numVertices, 0.0);
        tLinkSink.resize(numVertices, 0.0);

        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                int current = y * width + x;

                // Arestas para pixels vizinhos
                if (x + 1 < width)
                {
                    int right = y * width + (x + 1);
                    double weight = calculateWeight(image[y][x], image[y][x + 1], sigma);
                    addEdge(current, right, weight);
                    addEdge(right, current, weight);
                }

                if (y + 1 < height)
                {
                    int below = (y + 1) * width + x;
                    double weight = calculateWeight(image[y][x], image[y + 1][x], sigma);
                    addEdge(current, below, weight);
                    addEdge(below, current, weight);
                }

                // Configurar t-links
                double sourceWeight = -log(objProb[current]);
                double sinkWeight = -log(bgProb[current]);

                addEdge(source, current, sourceWeight); // Source -> Pixel
                addEdge(current, sink, sinkWeight);     // Pixel -> Sink
            }
        }
    }

    // Adiciona uma aresta ao grafo com capacidade
    void addEdge(int u, int v, double cap)
    {
        capacity[u][v] += cap;
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    // Algoritmo max-flow usando Edmonds-Karp
    double maxFlow()
    {
        vector<int> parent(numVertices);
        double flow = 0.0;

        while (bfs(parent))
        {
            double pathFlow = numeric_limits<double>::max();

            // Encontra a capacidade mínima ao longo do caminho aumentado
            for (int v = sink; v != source; v = parent[v])
            {
                int u = parent[v];
                pathFlow = min(pathFlow, capacity[u][v]);
            }

            // Atualiza as capacidades residuais das arestas e arestas reversas
            for (int v = sink; v != source; v = parent[v])
            {
                int u = parent[v];
                capacity[u][v] -= pathFlow;
                capacity[v][u] += pathFlow;
            }

            flow += pathFlow;
        }
        return flow;
    }

    // Retorna a segmentação dos pixels (se estão conectados ao source)
    vector<bool> segment()
    {
        vector<bool> segmentation(numVertices, false);
        vector<bool> visited(numVertices, false);
        queue<int> q;

        q.push(source);
        visited[source] = true;

        while (!q.empty())
        {
            int u = q.front();
            q.pop();
            segmentation[u] = true;

            for (int v : adj[u])
            {
                // Verificar se a aresta residual ainda tem capacidade
                if (!visited[v] && capacity[u][v] > 1e-6)
                {
                    visited[v] = true;
                    q.push(v);
                }
            }
        }

        // Retornar apenas os pixels válidos (excluindo source e sink)
        return vector<bool>(segmentation.begin(), segmentation.begin() + numVertices - 2);
    }

    // Exibe o grafo (usada apenas para debug)
    void printGraph() const
    {
        for (const auto &edge : edges)
        {
            cout << "Origem: " << edge.src
                 << ", Destino: " << edge.dest
                 << ", Peso: " << edge.weight << endl;
        }
        for (size_t i = 0; i < tLinkSource.size(); ++i)
        {
            cout << "Pixel " << i
                 << " -> Peso Source: " << tLinkSource[i]
                 << ", Peso Sink: " << tLinkSink[i] << endl;
        }
    }

private:
    // BFS para encontrar caminhos aumentantes
    bool bfs(vector<int> &parent)
    {
        fill(parent.begin(), parent.end(), -1);
        vector<bool> visited(numVertices, false);
        queue<int> q;

        q.push(source);
        visited[source] = true;

        while (!q.empty())
        {
            int u = q.front();
            q.pop();

            for (int v : adj[u])
            {
                if (!visited[v] && capacity[u][v] > 0)
                {
                    parent[v] = u;
                    visited[v] = true;

                    if (v == sink)
                        return true;
                    q.push(v);
                }
            }
        }

        return false;
    }

    // Calcula os pesos das arestas n-links
    double calculateWeight(const Pixel &p1, const Pixel &p2, double sigma)
    {
        double diff = sqrt(pow(p1.r - p2.r, 2) + pow(p1.g - p2.g, 2) + pow(p1.b - p2.b, 2));
        return exp(-diff * diff / (2 * sigma * sigma));
    }
};

#endif