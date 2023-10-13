

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

#pragma pack(1)

// Определяю структуру для представления заголовка BMP
struct BMPHeader {
    uint16_t signature;  //Это поле хранит сигнатуру BMP файла
    uint32_t fileSize;    //Это поле хранит размер всего файла BMP в байтах
    uint16_t reserved1; //Эти поля зарезервированы для будущего использования и обычно равны 0
    uint16_t reserved2;
    uint32_t dataOffset; //Это поле хранит смещение (в байтах) от начала файла до начала данных изображения в файле BMP
    uint32_t headerSize; //Это поле хранит размер заголовка файла BMP в байтах
    int32_t width;// Эти поля хранят ширину и высоту изображения в пикселях соответственно.
    int32_t height;
    uint16_t planes;// Это поле указывает количество цветовых плоскостей.
    uint16_t bitsPerPixel;//Это поле указывает количество бит на пиксель в изображении
    uint32_t compression;//Это поле указывает тип сжатия
    uint32_t dataSize; //Это поле хранит размер данных изображения (в байтах)
    int32_t horizontalRes; //Эти поля указывают на горизонтальное и вертикальное разрешение изображения в пикселях на метр
    int32_t verticalRes;
    uint32_t colors; //Это поле указывает на количество цветов в цветовой палитре
    uint32_t importantColors; //Это поле указывает на количество "важных" цветов в цветовой палитре
};

#pragma pack(pop)

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
                    int pixelOffset = ((y + j) * width + (x + i)) * 3;
                    double weight = kernel[j + radius][i + radius];

                    sumR += pixels[pixelOffset] * weight;
                    sumG += pixels[pixelOffset + 1] * weight;
                    sumB += pixels[pixelOffset + 2] * weight;
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

// Функция поворота изображения на 90 градусов
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


void rotateImage90DegreesBackward(std::vector<unsigned char>& pixels, int width, int height) {
    // Создаю новый вектор для хранения обратно повернутых пикселей
    std::vector<unsigned char> protatedPixels(pixels.size());

    // Вращаю пиксели изображения и выполняю обратное поворот
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            int srcOffset = (y * width + x) * 3;
            int destOffset = ((height - y - 1) * width + x) * 3;
            protatedPixels[destOffset] = pixels[srcOffset];
            protatedPixels[destOffset + 1] = pixels[srcOffset + 1];
            protatedPixels[destOffset + 2] = pixels[srcOffset + 2];
        }
    }
    // Обновляю входные пиксели обратно повернутыми значениями
    pixels = protatedPixels;
}
int main() {
    setlocale(LC_ALL, "RU");
    std::ifstream inputFile("Head.bmp", std::ios::binary);

    // Чтаю заголовок BMP
    BMPHeader header;
    inputFile.read(reinterpret_cast<char*>(&header), sizeof(header));

    // Проверка на кратность 4
    int width = header.width;
    int height = header.height;
    int rowSize = (width * header.bitsPerPixel / 8 + 3) & (~3);

    std::vector<unsigned char> pixels(rowSize * height);    
    inputFile.read(reinterpret_cast<char*>(pixels.data()), pixels.size());
    inputFile.close();

    // Поворачиваю изображение на 90 градусов
    rotateImage90Degrees(pixels, width, height);
    header.width = height;
    header.height = width;

    // Обновление размера файла в заголовке
    header.fileSize = sizeof(header) + pixels.size();


    std::ofstream outputFile("finish.bmp", std::ios::binary); // Создаем объкт класса

    outputFile.write(reinterpret_cast<char*>(&header), sizeof(header)); //Эта строка записывает байты, представляющие структуру header, в файл outputFile
    outputFile.write(reinterpret_cast<char*>(pixels.data()), pixels.size()); //Эта строка записывает байты, представляющие данные пикселей изображения, хранящиеся в векторе pixels, в файл outputFile
    outputFile.close(); //Эта строка закрывает файл outputFile

    std::cout << "BMP файл перевернут в файл finish.bmp" << std::endl;

    std::ifstream rotatedInputFile("finish.bmp", std::ios::binary);//Эта строка создает объект класса ifstream с именем rotatedInputFile и открывает файл "finish.bmp" для чтения в двоичном режиме 

    BMPHeader rotatedHeader; //Эта строка создает объект структуры
    rotatedInputFile.read(reinterpret_cast<char*>(&rotatedHeader), sizeof(rotatedHeader)); //Эта строка читает байты из файла rotatedInputFile и сохраняет их в структуре rotatedHeader

    int rotatedWidth = rotatedHeader.width; //Эти строки извлекают значения ширины и высоты изображения из заголовка BMP и сохраняют их в переменных rotatedWidth и rotatedHeight
    int rotatedHeight = rotatedHeader.height;
    int rotatedRowSize = (rotatedWidth * rotatedHeader.bitsPerPixel / 8 + 3) & (~3);

    std::vector<unsigned char> rotatedPixels(rotatedRowSize * rotatedHeight); //Создается объект с именем. Этот вектор предназначен для хранения последовательности беззнаковых байтов 
    rotatedInputFile.read(reinterpret_cast<char*>(rotatedPixels.data()), rotatedPixels.size()); //Происходит чтение данных из файла с использованием
    rotatedInputFile.close();

    // Применение фильтра Гаусса к повернутому изображению
    GaussianFilter(rotatedPixels, rotatedWidth, rotatedHeight);

    // Создайте файл для сохранения изображения после фильтра Гаусса
    std::ofstream outputGaussianFile("filtr_gaussa.bmp", std::ios::binary);

    // Записать заголовок
    outputGaussianFile.write(reinterpret_cast<char*>(&rotatedHeader), sizeof(rotatedHeader));

    // Записать пиксели после фильтрации
    outputGaussianFile.write(reinterpret_cast<char*>(rotatedPixels.data()), rotatedPixels.size());

    outputGaussianFile.close();

    std::cout << "Фильтр Гаусса применен" << std::endl;

    // Обратный поворот изображения на 90 градусов
    rotateImage90DegreesBackward(rotatedPixels, rotatedWidth, rotatedHeight);
    rotatedHeader.width = rotatedWidth; // Переписываем заголовок с измененными размерами
    rotatedHeader.height = rotatedHeight;

    // Создайте файл для сохранения изображения после обратного поворота
    std::ofstream outputPerevorotFile("perevorot.bmp", std::ios::binary);

    // Записать заголовок
    outputPerevorotFile.write(reinterpret_cast<char*>(&rotatedHeader), sizeof(rotatedHeader));

    // Записать пиксели после обратного поворота
    outputPerevorotFile.write(reinterpret_cast<char*>(rotatedPixels.data()), rotatedPixels.size());

    outputPerevorotFile.close();

    std::cout << "Обратный поворот и сохранение завершены" << std::endl;

    return 0;
}


