#ifndef LAB1_DECLARATIONS
#define LAB1_DECLARATIONS

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


int calculateRowSize(BMPHeader header);
BMP readBMP(std::string filename);
void writeBMP(std::string filename, BMP image);
BMP rotateClockwise(BMP image);
BMP rotateCounterclockwise(BMP image);
void applyGaussianBlur(BMP& image, double sigma);

#endif //LAB1_DECLARATIONS