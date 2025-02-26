#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <sstream>

using namespace std;

// Estrutura para armazenar um pixel
struct Pixel
{
    int r, g, b; // Valores de cor (0-255)
};

// Função para carregar uma imagem PPM (P3 ou P6)
vector<vector<Pixel>> loadPPM(const string &filename, int &width, int &height)
{
    ifstream file(filename, ios::binary);
    if (!file.is_open())
    {
        throw runtime_error("Erro ao abrir o arquivo PPM!");
    }

    string format;
    file >> format;

    // Verificar o formato PPM
    if (format != "P3" && format != "P6")
    {
        throw runtime_error("Formato PPM não suportado: " + format);
    }

    // Ler largura, altura e valor máximo
    int maxColorValue;
    file >> width >> height >> maxColorValue;
    file.ignore();

    if (maxColorValue != 255)
    {
        throw runtime_error("Apenas valores de cor máxima 255 são suportados!");
    }

    // Criar a matriz de pixels
    vector<vector<Pixel>> image(height, vector<Pixel>(width));

    if (format == "P3")
    {
        // Formato texto
        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                file >> image[y][x].r >> image[y][x].g >> image[y][x].b;
            }
        }
    }
    else if (format == "P6")
    {
        // Formato binário
        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                unsigned char rgb[3];
                file.read(reinterpret_cast<char *>(rgb), 3);
                image[y][x] = {rgb[0], rgb[1], rgb[2]};
            }
        }
    }

    return image;
}

#endif