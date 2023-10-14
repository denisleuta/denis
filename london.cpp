
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

#pragma pack(1)

// Определяю структуру для представления заголовка BMP
struct BMPHeader {
    uint16_t signature = 0x4D42;  //Это поле хранит сигнатуру BMP файла
    uint32_t fileSize = 0;    //Это поле хранит размер всего файла BMP в байтах
    uint16_t reserved1 = 0; //Эти поля зарезервированы для будущего использования и обычно равны 0
    uint16_t reserved2 = 0;
    uint32_t dataOffset = 54; //Это поле хранит смещение (в байтах) от начала файла до начала данных изображения в файле BMP
    uint32_t headerSize = 40; //Это поле хранит размер заголовка файла BMP в байтах
    int32_t width = 0;// Эти поля хранят ширину и высоту изображения в пикселях соответственно.
    int32_t height = 0;
    uint16_t planes = 1;// Это поле указывает количество цветовых плоскостей.
    uint16_t bitsPerPixel = 24;//Это поле указывает количество бит на пиксель в изображении
    uint32_t compression = 0;//Это поле указывает тип сжатия
    uint32_t dataSize = 0; //Это поле хранит размер данных изображения (в байтах)
    int32_t horizontalRes = 0; //Эти поля указывают на горизонтальное и вертикальное разрешение изображения в пикселях на метр
    int32_t verticalRes = 0;
    uint32_t colors = 0; //Это поле указывает на количество цветов в цветовой палитре
    uint32_t importantColors = 0; //Это поле указывает на количество "важных" цветов в цветовой палитре
};

#pragma pack(pop)

void ApplyFilter(const std::vector<unsigned char>& pixels, int width, int height, const double kernel[5][5], int radius, int x, int y, std::vector<unsigned char>& filteredPixels) {
    double sumR = 0.0, sumG = 0.0, sumB = 0.0;

    // Применяем ядро для вычисления отфильтрованных значений
    for (int j = -radius; j <= radius; j++) {
        for (int i = -radius; i <= radius; i++) {
            int pixelOffset = ((y + j) * width + (x + i)) * 3;
            double weight = kernel[j + radius][i + radius];

            sumR += pixels[pixelOffset] * weight;
            sumG += pixels[pixelOffset + 1] * weight;
            sumB += pixels[pixelOffset + 2] * weight;
        }
    }

    // Обновляем отфильтрованные значения пикселей
    int offset = (y * width + x) * 3;
    filteredPixels[offset] = static_cast<unsigned char>(sumR);
    filteredPixels[offset + 1] = static_cast<unsigned char>(sumG);
    filteredPixels[offset + 2] = static_cast<unsigned char>(sumB);
}

void GaussianFilter(std::vector<unsigned char>& pixels, int width, int height) {

    // Создаем новый вектор для хранения отфильтрованных пикселей
    std::vector<unsigned char> filteredPixels(pixels.size());

    // Матрица фильтра Гаусса 5x5
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
            ApplyFilter(pixels, width, height, kernel, radius, x, y, filteredPixels);
        }
    }

    // Обновляем входные пиксели с помощью отфильтрованных значений
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

bool ReadBMPHeader(const std::string& filename, BMPHeader& header) {
    std::ifstream inputFile(filename, std::ios::binary);

    if (!inputFile) {
        std::cerr << "Не удалось открыть " << filename << std::endl;
        return false;
    }

    inputFile.read(reinterpret_cast<char*>(&header), sizeof(header));

    if (!inputFile) {
        std::cerr << "Не удалось прочитать заголовок BMP " << filename << std::endl;
        return false;
    }

    return true;
}

int main() {
    setlocale(LC_ALL, "RU");
    std::ifstream inputFile("nose.bmp", std::ios::binary);

    // Чтаю заголовок BMP
    BMPHeader header;
    if (ReadBMPHeader("nose.bmp", header)) {

        // Проверка на кратность 4
        long int width = header.width;
        long int height = header.height;
        long int rowSize = ((width * header.bitsPerPixel + 31) / 32) * 4;

        std::vector<unsigned char> pixels(rowSize * height);
        inputFile.read(reinterpret_cast<char*>(pixels.data()), pixels.size());
        inputFile.close();

        // Поворачиваю изображение на 90 градусов
        rotateImage90Degrees(pixels, width, height);
        header.width = height;
        header.height = width;

        // Пересчитываем размер данных
        header.dataSize = static_cast<uint32_t>(pixels.size());

        // Обновление размера файла в заголовке
        header.fileSize = sizeof(header) + pixels.size();


        std::ofstream outputFile("finish.bmp", std::ios::binary); // Создаем объкт класса

        // Запись заголовка в файл
        outputFile.write(reinterpret_cast<char*>(&header), sizeof(header));

        // Запись пикселей после поворота
        outputFile.write(reinterpret_cast<char*>(pixels.data()), pixels.size());

        outputFile.close(); // Закрываем файл

        std::cout << "BMP файл перевернут в файл finish.bmp" << std::endl;


        std::ifstream rotatedInputFile("finish.bmp", std::ios::binary);//Эта строка создает объект класса ifstream с именем rotatedInputFile и открывает файл "finish.bmp" для чтения в двоичном режиме 

        BMPHeader rotatedHeader; //Эта строка создает объект структуры
        rotatedInputFile.read(reinterpret_cast<char*>(&rotatedHeader), sizeof(rotatedHeader)); //Эта строка читает байты из файла rotatedInputFile и сохраняет их в структуре rotatedHeader

        long int rotatedWidth = rotatedHeader.width; //Эти строки извлекают значения ширины и высоты изображения из заголовка BMP и сохраняют их в переменных rotatedWidth и rotatedHeight
        long int rotatedHeight = rotatedHeader.height;
        long int rotatedRowSize = ((rotatedWidth * rotatedHeader.bitsPerPixel + 31) / 32) * 4;

        std::vector<unsigned char> rotatedPixels(rotatedRowSize * rotatedHeight); //Создается объект с именем. Этот вектор предназначен для хранения последовательности беззнаковых байтов 
        rotatedInputFile.read(reinterpret_cast<char*>(rotatedPixels.data()), rotatedPixels.size()); //Происходит чтение данных из файла с использованием
        rotatedInputFile.close();

        // Примененяю фильтра Гаусса к повернутому изображению
        GaussianFilter(rotatedPixels, rotatedWidth, rotatedHeight);

        // Создаю файл для сохранения изображения после фильтра Гаусса
        std::ofstream outputGaussianFile("filtr_gaussa.bmp", std::ios::binary);

        // Записываю заголовок
        outputGaussianFile.write(reinterpret_cast<char*>(&rotatedHeader), sizeof(rotatedHeader));

        // Записываю пиксели после фильтрации
        outputGaussianFile.write(reinterpret_cast<char*>(rotatedPixels.data()), rotatedPixels.size());

        outputGaussianFile.close();

        std::cout << "Фильтр Гаусса применен" << std::endl;

        // Обратный поворот изображения на 90 градусов
        rotateImage90DegreesBackward(rotatedPixels, rotatedWidth, rotatedHeight);
        rotatedHeader.width = rotatedWidth; // Переписываюзаголовок с измененными размерами
        rotatedHeader.height = rotatedHeight;


        // Создаю файл для сохранения изображения после обратного поворота
        std::ofstream outputPerevorotFile("perevorot.bmp", std::ios::binary);

        // Записываю заголовок
        outputPerevorotFile.write(reinterpret_cast<char*>(&rotatedHeader), sizeof(rotatedHeader));

        // Записываю пиксели после обратного поворота
        outputPerevorotFile.write(reinterpret_cast<char*>(rotatedPixels.data()), rotatedPixels.size());

        outputPerevorotFile.close();

        std::cout << "Обратный поворот и сохранение завершены" << std::endl;
    }

    return 0;
}



