#include "graph.hpp"
#include "imageloader.hpp"
#include "maxflow.hpp"
#include "color_segments.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <cstring>
#include <fstream>
#include <chrono>

using namespace std;

// Função para verificar a existência de um arquivo
bool fileExists(const string &filePath)
{
    ifstream file(filePath);
    return file.good();
}

// Função para obter o nome do arquivo sem extensão
string getFileNameWithoutExtension(const string &filePath)
{
    size_t lastSlash = filePath.find_last_of("/\\");
    size_t lastDot = filePath.find_last_of(".");
    if (lastDot == string::npos || (lastSlash != string::npos && lastDot < lastSlash))
        return filePath.substr(lastSlash + 1);

    return filePath.substr(lastSlash + 1, lastDot - lastSlash - 1);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cerr << "Uso: segmentar <arquivo>" << endl;
        return -1;
    }

    string filename = argv[1];

    if (!fileExists(filename))
    {
        cerr << "Erro: Arquivo '" << filename << "' não encontrado." << endl;
        return -1;
    }

    string outputFilename = "images/saida/" + getFileNameWithoutExtension(filename) + "_out.ppm";

    int width, height;

    try
    {
        // Carregar a imagem e obter a matriz de pixels
        auto start = chrono::high_resolution_clock::now(); // Início da medição de tempo
        auto image = loadPPM(filename, width, height);
        auto end = chrono::high_resolution_clock::now();
        cout << "Tempo para carregar a imagem: " << chrono::duration<double>(end - start).count() << " segundos." << endl;

        double sigma = 10.0;

        vector<double> objProb(width * height, 0.5); // Probabilidade inicial de ser objeto
        vector<double> bgProb(width * height, 0.5);  // Probabilidade inicial de ser fundo

        vector<bool> isSeedObject(width * height, false);
        vector<bool> isSeedBackground(width * height, false);

        // Marcar alguns pixels manualmente como seeds
        isSeedObject[1] = true;
        objProb[1] = 0.99; // Alta probabilidade de objeto
        bgProb[1] = 0.01;

        isSeedBackground[15] = true;
        objProb[15] = 0.01; // Alta probabilidade de fundo
        bgProb[15] = 0.99;

        // Criar o grafo a partir da imagem
        start = chrono::high_resolution_clock::now();
        Graph graph(image, width, height, sigma, objProb, bgProb);
        end = chrono::high_resolution_clock::now();
        cout << "Tempo para criar o grafo: " << chrono::duration<double>(end - start).count() << " segundos." << endl;

        // Configurar os t-links
        for (int i = 0; i < width * height; ++i)
        {
            graph.tLinkSource[i] = -log(objProb[i]);
            graph.tLinkSink[i] = -log(bgProb[i]);

            if (isSeedObject[i])
            {
                graph.tLinkSource[i] = INFINITY; // Conecta ao source
                graph.tLinkSink[i] = 0;          // Não conecta ao sink
            }
            if (isSeedBackground[i])
            {
                graph.tLinkSource[i] = 0;      // Não conecta ao source
                graph.tLinkSink[i] = INFINITY; // Conecta ao sink
            }
        }

        // Calcular o fluxo máximo e obter o corte mínimo
        start = chrono::high_resolution_clock::now();
        double maxFlowValue = graph.maxFlow();
        end = chrono::high_resolution_clock::now();
        cout << "Fluxo maximo: " << maxFlowValue << endl;
        cout << "Tempo para calcular o fluxo maximo: " << chrono::duration<double>(end - start).count() << " segundos." << endl;

        // Análise de complexidade do cálculo do fluxo máximo:
        // O(E*f), onde E é o número de arestas e f é o valor do fluxo máximo.
        // Utiliza algoritmos como Edmonds-Karp ou Push-Relabel.

        start = chrono::high_resolution_clock::now();
        vector<bool> segmentation = graph.segment();
        end = chrono::high_resolution_clock::now();
        cout << "Segmentacao concluida. Tamanho: " << segmentation.size() << endl;
        cout << "Tempo para segmentar: " << chrono::duration<double>(end - start).count() << " segundos." << endl;

        start = chrono::high_resolution_clock::now();
        vector<vector<Pixel>> coloredImage = ImageSegmentColorizer::colorizeSegmentation(image, segmentation, width, height);
        ImageSegmentColorizer::savePPM(outputFilename, coloredImage, width, height);
        end = chrono::high_resolution_clock::now();
        cout << "Imagem segmentada salva em: " << outputFilename << endl;
        cout << "Tempo para salvar imagem segmentada: " << chrono::duration<double>(end - start).count() << " segundos." << endl;
    }
    catch (const exception &e)
    {
        cerr << "Erro: " << e.what() << endl;
        return -1;
    }

    return 0;
}

// **Complexity Analysis**
// * Loading Image: O(W * H), where W is width and H is height. Reads each pixel.
// * Graph Creation: O(W * H * 8), creating nodes and edges for each pixel and its neighbors.
// * Max-Flow Calculation: O(E * f), where E is the number of edges and f is the max flow value.
// * Segmentation: O(W * H), iterating over each pixel to determine its segment.
// * Colorization: O(W * H), assigning colors to each pixel based on its segment.
// * Saving Image: O(W * H), writing each pixel to the output file.

// * Overall: O(W * H * 8 + E * f), where E is the number of edges and f is the max flow value.

// Compile command:
// g++ main.cpp -o segmentar
// segmentar images/teste.ppm