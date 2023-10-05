#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>

#pragma pack(push, 1)
struct BMPHeader {
    uint16_t signature;
    uint32_t fileSize;
    uint32_t reserved;
    uint32_t dataOffset;
    uint32_t headerSize;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bitsPerPixel;
    uint32_t compression;
    uint32_t dataSize;
    int32_t horizontalResolution;
    int32_t verticalResolution;
    uint32_t colors;
    uint32_t importantColors;
};
#pragma pack(pop)

struct BMP {
    BMPHeader header;
    unsigned char* pixels;
};

int calculateRowSize(BMPHeader header) {
    int bytesPerPixel = header.bitsPerPixel / 8;
    int rowSizeBytes = (header.width * bytesPerPixel + 3) & ~3;
    return rowSizeBytes;
}

BMP readBMP(std::string filename) {
    BMP image;

    std::ifstream inputFile(filename, std::ios::binary);
    if (!inputFile) {
        throw std::runtime_error("Unable to open the BMP file for reading.");
    }

    inputFile.read((char*)(&image.header), sizeof(BMPHeader));

    int bytesToSkip = image.header.dataOffset - sizeof(BMPHeader);

    inputFile.seekg(bytesToSkip, std::ios::cur);

    int rowSize = calculateRowSize(image.header);
    int paddedSize = rowSize * image.header.height;

    image.pixels = new unsigned char[paddedSize];
    inputFile.read((char*)(image.pixels), paddedSize);

    inputFile.close();
    return image;
}

void writeBMP(std::string filename, BMP image) {
    std::ofstream outputFile(filename, std::ios::binary);
    if (!outputFile) {
        throw std::runtime_error("Unable to create the BMP file for writing.");
    }

    int rowSize = calculateRowSize(image.header);

    outputFile.write((char*)(&image.header), sizeof(BMPHeader));

    int bytesToWrite = image.header.dataOffset - sizeof(BMPHeader);
    for (int i = 0; i < bytesToWrite; i++) {
        char extraByte = 0;
        outputFile.write(&extraByte, 1);
    }

    for (int y = 0; y < image.header.height; y++) {
        int pixelOffset = y * rowSize;
        outputFile.write((char*)(image.pixels + pixelOffset), rowSize);
    }

    outputFile.close();
}

BMP rotateClockwise(BMP image) {
    BMP newImage;

    int bpp = image.header.bitsPerPixel / 8;

    newImage.header = image.header;
    newImage.header.width += newImage.header.height;
    newImage.header.height = newImage.header.width - newImage.header.height;
    newImage.header.width -= newImage.header.height;

    int newRowSize = ((newImage.header.width * bpp + 3) / 4) * 4;
    int oldRowSize = calculateRowSize(image.header);

    newImage.header.fileSize = sizeof(BMPHeader) + newRowSize * newImage.header.height;
    newImage.header.dataSize = newRowSize * newImage.header.height;

    newImage.pixels = new unsigned char[newImage.header.dataSize];

    for (int x = 0; x < newImage.header.height; x++) {
        for (int y = 0; y < newImage.header.width; y++) {
            int oldOffset = y * oldRowSize + x * bpp; 
            int newOffset = (newImage.header.height - 1 - x) * newRowSize + y * bpp;

            for (int channel = 0; channel < bpp; channel++) {
                newImage.pixels[newOffset + channel] = image.pixels[oldOffset + channel];
            }
        }
    }

    return newImage;
}

BMP rotateCounterclockwise(BMP image) {
    BMP newImage;

    int bpp = image.header.bitsPerPixel / 8; 

    newImage.header = image.header;
    newImage.header.width += newImage.header.height;
    newImage.header.height = newImage.header.width - newImage.header.height;
    newImage.header.width -= newImage.header.height;

    int newRowSize = ((newImage.header.width * bpp + 3) / 4) * 4;
    int oldRowSize = calculateRowSize(image.header);

    newImage.header.fileSize = sizeof(BMPHeader) + newRowSize * newImage.header.height;
    newImage.header.dataSize = newRowSize * newImage.header.height;

    newImage.pixels = new unsigned char[newImage.header.dataSize];

    for (int x = 0; x < newImage.header.height; x++) {
        for (int y = 0; y < newImage.header.width; y++) {
            int oldOffset = y * oldRowSize + x * bpp;
            int newOffset = x * newRowSize + (newImage.header.width - 1 - y) * bpp;

            for (int channel = 0; channel < bpp; channel++) {
                newImage.pixels[newOffset + channel] = image.pixels[oldOffset + channel];
            }
        }
    }

    return newImage;
}

void applyGaussianBlur(BMP& image, double sigma) {
    int bpp = image.header.bitsPerPixel / 8;

    int rowSize = ((image.header.width * bpp + 3) / 4) * 4;

    int kernelSize = (6 * sigma) + 1;
    double *kernel = new double[kernelSize];

    for (int i = 0; i < kernelSize; i++) {
        double x = i - (kernelSize - 1) / 2;
        double value = std::exp(-(x * x) / (2 * sigma * sigma));
        kernel[i] = value;
    }

    double sum = 0.0;
    for (int i = 0; i < kernelSize; i++) {
        sum += (kernel[i]);
    }
    for (int i = 0; i < kernelSize; i++) {
        kernel[i] /= sum;
    }

    BMP blurredImage = image;

    for (int y = 0; y < image.header.height; y++) {
        for (int x = 0; x < image.header.width; x++) {
            std::vector<double> colorChannels(bpp, 0.0);

            for (int i = 0; i < kernelSize; i++) {
                int posX = x + i - (kernelSize - 1) / 2;
                if (posX < 0) posX = 0;
                if (posX >= image.header.width) posX = image.header.width - 1;

                int pixelOffset = y * rowSize + posX * bpp;

                for (int channel = 0; channel < bpp; channel++) {
                    colorChannels[channel] +=
                        (double)(image.pixels[pixelOffset + channel]) * kernel[i];
                }
            }

            int pixelOffset = y * rowSize + x * bpp;
            for (int channel = 0; channel < bpp; channel++) {
                blurredImage.pixels[pixelOffset + channel] =
                    (unsigned char)(colorChannels[channel]);
            }
        }
    }

    delete[] kernel;
    kernel = nullptr;
    image = blurredImage;
}

int main() {
    try {
        BMP image = readBMP("test2.bmp");

        BMP rotatedImageRight = rotateClockwise(image);
        writeBMP("outputRight.bmp", rotatedImageRight);
        delete[] rotatedImageRight.pixels;
        rotatedImageRight.pixels = nullptr;
        std::cout << "Image rotated 90 degrees clockwise and saved as outputRight.bmp." << std::endl;

        BMP rotatedImageLeft = rotateCounterclockwise(image);
        writeBMP("outputLeft.bmp", rotatedImageLeft);
        delete[] rotatedImageLeft.pixels;
        rotatedImageLeft.pixels = nullptr;
        std::cout << "Image rotated 90 degrees counterclockwise and saved as outputLeft.bmp." << std::endl;

        double sigma = 7.0; // Adjust the sigma value as needed
        applyGaussianBlur(image, sigma);
        writeBMP("outputBlur.bmp", image);
        delete[] image.pixels;
        image.pixels = nullptr;
        std::cout << "Gaussian blur applied and saved as outputBlur.bmp." << std::endl;

    } catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
