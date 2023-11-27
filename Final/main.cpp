//
// file: main.cpp
// desc: ACS Final Project MAIN
// auth: Andrew Prata
//
// This program implements basic image processing
// functionality, emphasizing edge-detection.
//

#include "Grayscale.h" // Image data structure

int main(void) {

    // Random testing for now, functions as intended!

    Grayscale image(100, 100);
    image.setPixel(50, 50, 255);
    unsigned char pixval = image.getPixel(50, 50);
    std::cout << "val is " << (int)pixval << " out of 255" << std::endl;

    std::string filename = "testimage.png";
    Grayscale image2(filename);
    pixval = image2.getPixel(2, 2);
    std::cout << "val is " << (int)pixval << " out of 255" << std::endl;

    return 0;
}