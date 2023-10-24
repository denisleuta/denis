#include <iostream>
#include <fstream>
#include <vector>

#pragma pack(1) 
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

void readBMP(const std::string& filename, BMPHeader& header, std::vector<unsigned char>& pixels)
{
    // Открываем BMP-файл для бинарного чтения.
    std::ifstream File(filename, std::ios::binary);
    if (!File)
    {
        // Если файл не удалось открыть, выводим сообщение об ошибке и завершаем программу.
        std::cerr << "Error: Unable to open input file." << std::endl;
        exit(1);
    }

    // Считываем заголовок BMP-файла в структуру BMPHeader.
    File.read(reinterpret_cast<char*>(&header), sizeof(header));

    // Извлекаем ширину, высоту и размер строки из заголовка.
    int width = header.width;
    int height = header.height;
    int rowSize = (width * header.bitsPerPixel / 8 + 3) & (~3);

    // Выделяем память в векторе для хранения пикселей.
    pixels.resize(rowSize * height);

    // Считываем пиксели из файла в вектор.
    File.read(reinterpret_cast<char*>(pixels.data()), pixels.size());

    // Закрываем файл.
    File.close();
}

void writeBMP(const std::string& filename, const BMPHeader& header, const std::vector<unsigned char>& pixels)
{
    // Открываем выходной BMP-файл для бинарной записи.
    std::ofstream outputFile(filename, std::ios::binary);
    if (!outputFile)
    {
        // Если файл не удалось открыть, выводим сообщение об ошибке и завершаем программу.
        std::cerr << "Error: Unable to open output file." << std::endl;
        exit(1);
    }

    // Записываем заголовок BMP-файла в выходной файл.
    outputFile.write(reinterpret_cast<const char*>(&header), sizeof(header));

    // Записываем пиксели из вектора в выходной файл.
    outputFile.write(reinterpret_cast<const char*>(pixels.data()), pixels.size());

    // Закрываем выходной файл после записи данных.
    outputFile.close();
}


void rotateImage(std::vector<unsigned char>& pixels, int& width, int& height) {
    // Создаем временный вектор для хранения повернутых пикселей.
    std::vector<unsigned char> rotatedPixels(pixels.size());

    // Проходим по пикселям изображения.
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            // Вычисляем смещение для исходного пикселя.
            int srcOffset = (y * width + x) * 3;

            // Вычисляем смещение для целевого (повернутого) пикселя.
            int destOffset = ((width - x - 1) * height + y) * 3;

            // Копируем значения цветов исходного пикселя в целевой пиксель.
            rotatedPixels[destOffset] = pixels[srcOffset];
            rotatedPixels[destOffset + 1] = pixels[srcOffset + 1];
            rotatedPixels[destOffset + 2] = pixels[srcOffset + 2];
        }
    }

    // Заменяем исходный вектор пикселей на повернутый вектор.
    pixels = rotatedPixels;

    // Меняем ширину и высоту изображения местами, так как изображение повернуто.
    std::swap(width, height);
}


void GaussianFilter(std::vector<unsigned char>& pixels, int width, int height) {
    // Определяем ядро (какой-то фильтр) для гауссовского размытия.
    double kernel[5][5] = {
        {0.003, 0.013, 0.022, 0.013, 0.003},
        {0.013, 0.059, 0.097, 0.059, 0.013},
        {0.022, 0.097, 0.159, 0.097, 0.022},
        {0.013, 0.059, 0.097, 0.059, 0.013},
        {0.003, 0.013, 0.022, 0.013, 0.003}
    };

    int radius = 2; // Радиус ядра (половина его размера).
    std::vector<unsigned char> filteredPixels(pixels.size()); // Создаем временный вектор для хранения отфильтрованных пикселей.

    // Проходим по пикселям изображения, начиная с пикселей, находящихся внутри радиуса.
    for (int y = radius; y < height - radius; y++) {
        for (int x = radius; x < width - radius; x++) {
            // Инициализируем переменные для хранения сумм красной, зеленой и синей составляющих пикселя.
            double sumR = 0.0, sumG = 0.0, sumB = 0.0;

            ApplyFilter(pixels, width, height, kernel, radius, x, y, filteredPixels);
        }
    }

    // Обновляем входные пиксели с отфильтрованными значениями.
    pixels = filteredPixels;
}


int main()
{
    setlocale(LC_ALL, "RU");
    BMPHeader header;
    std::vector<unsigned char> pixels;

    readBMP("nose.bmp", header, pixels);

    rotateImage(pixels, header.width, header.height);

    header.fileSize = sizeof(header) + pixels.size();
    writeBMP("finish.bmp", header, pixels);

    std::cout << "BMP перевернут и загружен в файл finish.bmp" << std::endl;

    GaussianFilter(pixels, header.width, header.height);
    writeBMP("finishG.bmp", header, pixels);

    std::cout << "Фильтр гаусса применен" << std::endl;

    rotateImage(pixels, header.width, header.height);

    header.fileSize = sizeof(header) + pixels.size();
    writeBMP("pervor.bmp", header, pixels);
    std::cout << "Картинка перевернута" << std::endl;

    return 0;
}
