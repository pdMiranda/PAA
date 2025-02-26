#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include "imageloader.hpp" // Inclua o arquivo com a funcao loadPPM

using namespace std;

// Estrutura para uma aresta do grafo
struct Edge
{
    int src;        // Vértice de origem
    int dest;       // Vértice de destino
    double weightR; // Peso da aresta para o canal R
    double weightG; // Peso da aresta para o canal G
    double weightB; // Peso da aresta para o canal B
};

// Classe para representar o grafo
class Graph
{
public:
    int numVertices;    // Número total de vértices (pixels)
    vector<Edge> edges; // Lista de arestas

    // Construtor para criar o grafo a partir da imagem
    Graph(const vector<vector<Pixel>> &image, int width, int height)
    {
        numVertices = width * height;

        // Adicionar arestas baseadas em pixels vizinhos
        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                int current = y * width + x;

                // Adicionar aresta para o pixel da direita
                if (x + 1 < width)
                {
                    int right = y * width + (x + 1);
                    auto weights = calculateWeights(image[y][x], image[y][x + 1]);
                    edges.push_back({current, right, weights[0], weights[1], weights[2]});
                }

                // Adicionar aresta para o pixel abaixo
                if (y + 1 < height)
                {
                    int below = (y + 1) * width + x;
                    auto weights = calculateWeights(image[y][x], image[y + 1][x]);
                    edges.push_back({current, below, weights[0], weights[1], weights[2]});
                }

                // Opcional: Adicionar arestas diagonais (inferior direito)
                if (x + 1 < width && y + 1 < height)
                {
                    int diagRight = (y + 1) * width + (x + 1);
                    auto weights = calculateWeights(image[y][x], image[y + 1][x + 1]);
                    edges.push_back({current, diagRight, weights[0], weights[1], weights[2]});
                }

                // Opcional: Adicionar arestas diagonais (inferior esquerdo)
                if (x - 1 >= 0 && y + 1 < height)
                {
                    int diagLeft = (y + 1) * width + (x - 1);
                    auto weights = calculateWeights(image[y][x], image[y + 1][x - 1]);
                    edges.push_back({current, diagLeft, weights[0], weights[1], weights[2]});
                }
            }
        }
    }

    // Função para exibir o grafo
    void printGraph() const
    {
        for (const auto &edge : edges)
        {
            cout << "Origem: " << edge.src
                 << ", Destino: " << edge.dest
                 << ", Peso R: " << edge.weightR
                 << ", Peso G: " << edge.weightG
                 << ", Peso B: " << edge.weightB << endl;
        }
    }

    // Função para calcular os pesos de uma aresta com base nas diferenças de intensidade
    vector<double> calculateWeights(const Pixel &p1, const Pixel &p2)
    {
        double diffR = abs(p1.r - p2.r);
        double diffG = abs(p1.g - p2.g);
        double diffB = abs(p1.b - p2.b);
        return {diffR, diffG, diffB};
    }
};

#endif