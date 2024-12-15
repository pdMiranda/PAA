#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <sstream>

#include "imageloader.hpp"
#include "graph.hpp"
#include "component.hpp"
#include "color_components.hpp"

using namespace std;

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
    if (argc != 2) // Esperamos apenas um argumento: o nome do arquivo
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

    try
    {
        // Carregar a imagem e processar
        auto image = loadPPM(inputFile, width, height);
        Graph graph(image, width, height);

        double K = 300.0;
        GraphComponents components(graph.numVertices);
        components.segmentGraph(graph, K);

        int componentesnum = components.getNumberOfComponents();
        cout << "Numero de componentes detectados: " << componentesnum << endl;

        auto coloredImage = ImageColorizer::colorComponents(image, components, width, height);

        // Gerar nome do arquivo de saída
        string outputDir = "images/saida/";
        string inputFileName = getFileNameWithoutExtension(inputFile);
        string outputFile = outputDir + inputFileName + "_out.ppm";

        ImageColorizer::savePPM(outputFile, coloredImage, width, height);
        cout << "Imagem segmentada salva como '" << outputFile << "'." << endl;
    }
    catch (const exception &e)
    {
        cerr << "Erro: " << e.what() << endl;
        return -1;
    }

    return 0;
}

// g++ main.cpp -o segmentar
// segmentar images/teste.ppm