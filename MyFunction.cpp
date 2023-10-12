#include "MyFunction.h"

void GaussianFilter(std::vector<unsigned char>& pixels, int width, int height) {
    // Создаю новый вектор для хранения отфильтрованных пикселей
    std::vector<unsigned char> filteredPixels(pixels.size());

    // Матрица фильра Гаусса 5x5
    double kernel[5][5] = {
        {0.003, 0.013, 0.022, 0.013, 0.003},
        {0.013, 0.059, 0.097, 0.059, 0.013},
        {0.022, 0.097, 0.159, 0.097, 0.022},
        {0.013, 0.059, 0.097, 0.059, 0.013},
        {0.003, 0.013, 0.022, 0.013, 0.003}
    };

    int radius = 2;

    // Цикл по пикселям изображения
    for (int y = radius; y < height - radius; y++) {
        for (int x = radius; x < width - radius; x++) {
            double sumR = 0.0, sumG = 0.0, sumB = 0.0;

            // Применяю ядро для вычисления отфильтрованных значений
            for (int j = -radius; j <= radius; j++) {
                for (int i = -radius; i <= radius; i++) {
                    if (x >= 0 && x < width && y >= 0 && y < height) {
                        int pixelOffset = ((y + j) * width + (x + i)) * 3;
                        double weight = kernel[j + radius][i + radius];

                        sumR += pixels[pixelOffset] * weight;
                        sumG += pixels[pixelOffset + 1] * weight;
                        sumB += pixels[pixelOffset + 2] * weight;
                    }
                }
            }
            // Обновляю отфильтрованные значения пикселей
            int offset = (y * width + x) * 3;
            filteredPixels[offset] = static_cast<unsigned char>(sumR);
            filteredPixels[offset + 1] = static_cast<unsigned char>(sumG);
            filteredPixels[offset + 2] = static_cast<unsigned char>(sumB);
        }
    }
    // Обновляю входные пиксели с помощью отфильтрованных значений
    pixels = filteredPixels;
}

void rotateImage90Degrees(std::vector<unsigned char>& pixels, int width, int height) {

     // Создаю новый вектор для хранения повернутых пикселей
    std::vector<unsigned char> rotatedPixels(pixels.size());

    // Вращаю пиксели изображения и выполняю поворот
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            int srcOffset = (y * width + x) * 3;
            int destOffset = ((width - x - 1) * height + y) * 3;
            rotatedPixels[destOffset] = pixels[srcOffset];
            rotatedPixels[destOffset + 1] = pixels[srcOffset + 1];
            rotatedPixels[destOffset + 2] = pixels[srcOffset + 2];
        }
    }
    // Обновляю входные пиксели повернутыми значениями
    pixels = rotatedPixels;
}
