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
    int src;       // Vértice de origem
    int dest;      // Vértice de destino
    double weight; // Peso da aresta
};

// Classe para representar o grafo
class Graph
{
public:
    int numVertices;            // Número total de vértices (pixels)
    vector<Edge> edges;         // Lista de arestas
    vector<double> tLinkSource; // Pesos para os t-links conectando ao source
    vector<double> tLinkSink;   // Pesos para os t-links conectando ao sink

    // Construtor para criar o grafo a partir da imagem
    Graph(const vector<vector<Pixel>> &image, int width, int height, double sigma, const vector<double> &objProb, const vector<double> &bgProb)
    {
        numVertices = width * height;
        tLinkSource.resize(numVertices, 0.0);
        tLinkSink.resize(numVertices, 0.0);

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
                    double weight = calculateWeight(image[y][x], image[y][x + 1], sigma); // w_{p,q} = exp(-|I_p - I_q|^2 / (2 * sigma^2))
                    edges.push_back({current, right, weight});
                }

                // Adicionar aresta para o pixel abaixo
                if (y + 1 < height)
                {
                    int below = (y + 1) * width + x;
                    double weight = calculateWeight(image[y][x], image[y + 1][x], sigma); // w_{p,q} = exp(-|I_p - I_q|^2 / (2 * sigma^2))
                    edges.push_back({current, below, weight});
                }

                // Configurar t-links usando as probabilidades de objeto e fundo
                tLinkSource[current] = -log(objProb[current]); // w_{p,S} = -ln(Pr(I_p | objeto))
                tLinkSink[current] = -log(bgProb[current]);    // w_{p,T} = -ln(Pr(I_p | fundo))
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
    // Função para calcular os pesos das arestas n-links (termo de borda)
    double calculateWeight(const Pixel &p1, const Pixel &p2, double sigma)
    {
        double diff = sqrt(pow(p1.r - p2.r, 2) + pow(p1.g - p2.g, 2) + pow(p1.b - p2.b, 2)); // |I_p - I_q|
        return exp(-diff * diff / (2 * sigma * sigma));                                      // w_{p,q} = exp(-|I_p - I_q|^2 / (2 * sigma^2))
    }
};

#endif