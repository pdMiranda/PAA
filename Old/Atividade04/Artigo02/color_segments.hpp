#ifndef IMAGE_SEGMENT_COLORIZER_H
#define IMAGE_SEGMENT_COLORIZER_H

#include <vector>
#include <string>
#include <stdexcept>
#include <fstream>
#include "imageloader.hpp" // Para salvar a nova imagem
#include "graph.hpp"

using namespace std;

class ImageSegmentColorizer
{
public:
    // Função para gerar uma imagem colorida a partir da segmentação
    static vector<vector<Pixel>> colorizeSegmentation(const vector<vector<Pixel>> &image, const vector<bool> &segmentation, int width, int height)
    {
        Pixel objectColor = {255, 0, 0};     // Vermelho para o objeto
        Pixel backgroundColor = {0, 0, 255}; // Azul para o fundo

        vector<vector<Pixel>> coloredImage(height, vector<Pixel>(width));

        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                int vertex = y * width + x;
                if (segmentation[vertex])
                {
                    coloredImage[y][x] = backgroundColor;
                }
                else
                {
                    coloredImage[y][x] = objectColor;
                }
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