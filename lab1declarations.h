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
/* На первый раз это абсолютно нормальное и даже хорошее решение. Однако нам 
 * нужно добиться ООП. Конкретно тут нарушается один из его основных принципов:
 * инкапсуляция. Инкапсуляция -- это механизм сокрытия, позволяющий поддерживать
 * инварианты. В случае этой структуры может произойти так, что в глубине огромной
 * программы придет программист и нечаянно (он не в курсе, что вообще такое бмп)
 * изменит значение хэдера так, что картинка испортится. Нам нельзя давать ему такую 
 * возможность вовсе. Вот тут приходят на помощь классы и инкапсуляция, которые позволяют
 * работать с объектом только через публичный интерфейс, который мы проектировали так,
 * что он не может сломать ничего никак (а еще им удобно пользоваться). Поэтому к 
 * следующей лабе разберись, как хорошо оформлять классы */
struct BMP {
    BMPHeader header;
    unsigned char* pixels;
};

/* Если бы это был класс, то ребята ниже были бы методами и не прищлось бы передавать
 * картинку и хэдер как аргументы. */
int calculateRowSize(BMPHeader header);
BMP readBMP(std::string filename);
void writeBMP(std::string filename, BMP image);
BMP rotateClockwise(BMP image);
BMP rotateCounterclockwise(BMP image);
void applyGaussianBlur(BMP& image, double sigma);

#endif //LAB1_DECLARATIONS
