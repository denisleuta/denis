#pragma once

#include <vector>

struct BMPHeader {
    uint16_t signature;
    uint32_t fileSize;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t dataOffset;
    uint32_t headerSize;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bitsPerPixel;
    uint32_t compression;
    uint32_t dataSize;
    int32_t horizontalRes;
    int32_t verticalRes;
    uint32_t colors;
    uint32_t importantColors;
};

void GaussianFilter(std::vector<unsigned char>& pixels, int width, int height);
void rotateImage90Degrees(std::vector<unsigned char>& pixels, int width, int height);


