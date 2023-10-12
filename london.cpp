#include <iostream>
#include <fstream>
#include <vector>
#include "MyFunction.h"

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

    std::ofstream outputGaussianFile("filtr_gaussa.bmp", std::ios::binary); //Эта строка создает объект outputGaussianFile типа std::ofstream для записи данных в файл с именем "filtr_gaussa.bmp"

    outputGaussianFile.write(reinterpret_cast<char*>(&rotatedHeader), sizeof(rotatedHeader)); //Эта строка записывает бинарные данные, представленные в структуре rotatedHeader, в файл outputGaussianFile
    outputGaussianFile.write(reinterpret_cast<char*>(rotatedPixels.data()), rotatedPixels.size()); //Эта строка работает аналогично предыдущей строке, только вместо структуры используется вектор
    outputGaussianFile.close();

    std::cout << "Фильтр Гаусса применен" << std::endl;

    return 0;
}
