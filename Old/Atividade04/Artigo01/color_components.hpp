#ifndef COLOR_COMPONENTS_H
#define COLOR_COMPONENTS_H

#include <vector>
#include <random>
#include "imageloader.hpp"
#include "graph.hpp"
#include "component.hpp"
#include <unordered_map>

using namespace std;

class ImageColorizer
{
public:
    // Função para gerar cores aleatórias
    static Pixel generateRandomColor()
    {
        static random_device rd;
        static mt19937 gen(rd());
        static uniform_int_distribution<> dis(0, 255);
        return {dis(gen), dis(gen), dis(gen)};
    }

    // Colorir os componentes do grafo
    static vector<vector<Pixel>> colorComponents(const vector<vector<Pixel>> &image, GraphComponents &components, int width, int height)
    {
        // Mapear cada componente a uma cor aleatória
        unordered_map<int, Pixel> componentColors;
        for (int i = 0; i < components.parent.size(); ++i)
        {
            int root = components.find(i);
            if (componentColors.find(root) == componentColors.end())
            {
                componentColors[root] = generateRandomColor();
            }
        }

        // Criar a nova imagem colorida
        vector<vector<Pixel>> coloredImage(height, vector<Pixel>(width));

        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                int vertex = y * width + x;
                int root = components.find(vertex);
                coloredImage[y][x] = componentColors[root];
            }
        }

        return coloredImage;
    }

    // Salvar a nova imagem em formato PPM (P3)
    static void savePPM(const string &filename, const vector<vector<Pixel>> &image, int width, int height)
    {
        ofstream file(filename);
        if (!file.is_open())
        {
            throw runtime_error("Erro ao salvar a imagem: " + filename);
        }

        file << "P3\n";
        file << width << " " << height << "\n255\n";

        for (const auto &row : image)
        {
            for (const auto &pixel : row)
            {
                file << pixel.r << " " << pixel.g << " " << pixel.b << " ";
            }
            file << "\n";
        }

        file.close();
    }
};

#endif