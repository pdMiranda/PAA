#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <chrono>
#include "imageloader.hpp"
#include "graph.hpp"
#include "component.hpp"
#include "color_components.hpp"

using namespace std;
using namespace chrono;

// Função para imprimir a matriz de pixels
void printImage(const vector<vector<Pixel>> &image, int width, int height)
{
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            const Pixel &p = image[y][x];
            cout << "(" << p.r << ", " << p.g << ", " << p.b << ")";
            if (x < width - 1)
                cout << " "; // Espaço entre pixels
        }
        cout << "\n"; // Nova linha após cada linha de pixels
    }
}

// Função para mostrar todos os pixels
void printPixels(const vector<vector<Pixel>> &image, int width, int height)
{
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            cout << "Pixel [" << y << "][" << x << "]: "
                 << "R=" << image[y][x].r << ", "
                 << "G=" << image[y][x].g << ", "
                 << "B=" << image[y][x].b << endl;
        }
    }
}

string getFileNameWithoutExtension(const string &filePath)
{
    size_t lastSlash = filePath.find_last_of("/\\");
    size_t lastDot = filePath.find_last_of(".");
    if (lastDot == string::npos || (lastSlash != string::npos && lastDot < lastSlash))
        return filePath.substr(lastSlash + 1);

    return filePath.substr(lastSlash + 1, lastDot - lastSlash - 1);
}

bool fileExists(const string &filePath)
{
    ifstream file(filePath);
    return file.good();
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cerr << "Uso: segmentar <arquivo>" << endl;
        return -1;
    }

    string inputFile = argv[1];
    int width, height;

    // Validar se o arquivo existe
    if (!fileExists(inputFile))
    {
        cerr << "Erro: Arquivo '" << inputFile << "' não encontrado." << endl;
        return -1;
    }

    string outputDir = "images/saida/";
    string inputFileName = getFileNameWithoutExtension(inputFile);
    string outputFile = outputDir + inputFileName + "_out.ppm";

    try
    {
        // Medir o tempo de execução geral do programa
        auto start = high_resolution_clock::now();

        // Carregar a imagem
        auto loadStart = high_resolution_clock::now();
        auto image = loadPPM(inputFile, width, height);
        auto loadEnd = high_resolution_clock::now();

        // Criar o grafo da imagem
        auto graphStart = high_resolution_clock::now();
        Graph graph(image, width, height);
        auto graphEnd = high_resolution_clock::now();

        // Segmentar o grafo
        auto colorStart = high_resolution_clock::now();

        double K = 300.0;
        GraphComponents components(graph.numVertices);
        components.segmentGraph(graph, K);

        auto coloredImage = ImageColorizer::colorComponents(image, components, width, height);

        auto colorEnd = high_resolution_clock::now();

        // Salvar a imagem segmentada
        auto saveStart = high_resolution_clock::now();
        ImageColorizer::savePPM(outputFile, coloredImage, width, height);
        auto saveEnd = high_resolution_clock::now();

        // Exibir tempo total
        auto end = high_resolution_clock::now();

        // Tempos de execução em nanosegundos
        cout << "Tempo de carga: " << duration_cast<nanoseconds>(loadEnd - loadStart).count() << "ns." << endl;
        cout << "Tempo de criacao do grafo: " << duration_cast<nanoseconds>(graphEnd - graphStart).count() << "ns." << endl;
        cout << "Tempo de segmentacao: " << duration_cast<nanoseconds>(colorEnd - colorStart).count() << "ns." << endl;
        cout << "Tempo de salvamento: " << duration_cast<nanoseconds>(saveEnd - saveStart).count() << "ns." << endl;
        cout << "Tempo total: " << duration_cast<nanoseconds>(end - start).count() << "ns." << endl;
    }
    catch (const exception &e)
    {
        cerr << "Erro: " << e.what() << endl;
        return -1;
    }

    return 0;
}

// **Complexity Analysis
// * - Loading Image: O(W * H), where W is the width and H is the height of the image. Each pixel is read from the file.
// * - Graph Creation: O(W * H * 8), where W is the width and H is the height. For each pixel, a node is created and edges are added to neighboring pixels (up to 8 neighbors).
// * - Segmentation: O(K * n * d), where K is the number of clusters, n is the number of pixels (n = W * H), and d is the number of dimensions (typically 3 for RGB colors).
// * - Colorization: O(W * H), where W is the width and H is the height. Each pixel is processed to assign the appropriate color based on the segment.
// * - Saving Image: O(W * H), where W is the width and H is the height. Each pixel is written to the output file.

// * - Overall, the complexity of the segmentation algorithm depends on the number of clusters and the number of pixels in the image. The colorization and saving steps are linear with respect to the number of pixels in the image.

// Compile command:
// g++ main.cpp -o segmentar
// segmentar images/teste.ppm