#include <iostream>
#include "lab1declarations.h"

int main() {
    /* На самом деле обработка исключений и вообще их использование -- спорный топик. 
     * Например в код стайле гугла они вовсе запрещены. Однако я тоже считаю, что в этой
     * программе они смотрятся хорошо. Но просто ради самообразования почитай в интернетике,
     * чем они могут быть плохи (но плюсов у них точно больше) */
    try {
        BMP image = readBMP("test1.bmp");

        BMP rotatedImageRight = rotateClockwise(image);
        writeBMP("outputRight.bmp", rotatedImageRight);
        delete[] rotatedImageRight.pixels;
        rotatedImageRight.pixels = nullptr; //Очень хорошо
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
