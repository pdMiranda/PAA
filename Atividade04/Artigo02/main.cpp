#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <cmath>
#include "graph.hpp"
#include "MaxFlowSolver.hpp"
#include "imageloader.hpp"

using namespace std;

string getFileNameWithoutExtension(const string &filePath)
{
    size_t lastSlash = filePath.find_last_of("/");
    size_t lastDot = filePath.find_last_of(".");

    if (lastDot == string::npos || (lastSlash != string::npos && lastDot < lastSlash))
        return filePath.substr(lastSlash + 1); // Sem extensão

    return filePath.substr(lastSlash + 1, lastDot - lastSlash - 1);
}

void savePPM(const string &filename, const vector<vector<Pixel>> &image, int width, int height)
{
    ofstream file(filename, ios::binary);
    if (!file.is_open())
    {
        throw runtime_error("Erro ao salvar o arquivo PPM!");
    }

    // Escrever o cabeçalho
    file << "P6\n"
         << width << " " << height << "\n255\n";

    // Escrever os dados da imagem
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            unsigned char rgb[3] = {static_cast<unsigned char>(image[y][x].r),
                                    static_cast<unsigned char>(image[y][x].g),
                                    static_cast<unsigned char>(image[y][x].b)};
            file.write(reinterpret_cast<char *>(rgb), 3);
        }
    }

    file.close();
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <filename>\n";
        return 1;
    }

    std::string filename = argv[1];
    int width, height;

    try
    {
        // Carregar a imagem e obter a matriz de pixels
        auto image = loadPPM(filename, width, height);

        double sigma = 50.0;

        vector<double> objProb(width * height, 0.7); // Probabilidade inicial de ser objeto
        vector<double> bgProb(width * height, 0.3);  // Probabilidade inicial de ser fundo

        vector<bool> isSeedObject(width * height, false);
        vector<bool> isSeedBackground(width * height, false);

        // Exemplo: marque alguns pixels manualmente como seeds
        isSeedObject[2] = true;
        isSeedBackground[8] = true;

        // Criar o grafo a partir da imagem
        int numVertices = width * height;
        MaxFlowSolver solver(numVertices);

        // Adicionar arestas ao solver com base na imagem
        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                int current = y * width + x;
                // Adicionar arestas com base nos vizinhos da imagem
                if (x + 1 < width)
                {
                    int right = y * width + (x + 1);
                    double weight = exp(-pow(image[y][x].r - image[y][x + 1].r, 2) / (2 * sigma * sigma));
                    solver.addEdge(current, right, weight);
                }

                if (y + 1 < height)
                {
                    int below = (y + 1) * width + x;
                    double weight = exp(-pow(image[y][x].r - image[y + 1][x].r, 2) / (2 * sigma * sigma));
                    solver.addEdge(current, below, weight);
                }
            }
        }

        // Realizar a segmentação com o método de fluxo máximo
        double maxFlow = solver.edmondsKarp(0, width * height - 1); // Source = 0, Sink = último pixel

        cout << "Fluxo maximo: " << maxFlow << endl;

        // Obter o nome do arquivo sem a extensão
        string baseFilename = getFileNameWithoutExtension(filename);
        string outputFilename = "images/saida/" + baseFilename + "_out.ppm"; // Adiciona "_out.ppm" no final do nome

        // Exemplo de como salvar a imagem processada
        savePPM(outputFilename, image, width, height);
    }
    catch (const std::exception &e)
    {
        cerr << "Erro: " << e.what() << endl;
        return 1;
    }

    return 0;
}
