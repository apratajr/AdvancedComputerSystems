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

    // Grayscale image(100, 100);
    // image.setPixel(50, 50, 255);
    // unsigned char pixval = image.getPixel(50, 50);
    // std::cout << "val is " << (int)pixval << " out of 255" << std::endl;

    std::string filename = "mario2.png";
    Grayscale image(filename);

    filename = "mario2grayscale.png";
    image.exportPNG(filename);

    Grayscale image_blur = gaussianBlurStrong(image);
    filename = "mario2blur.png";
    image_blur.exportPNG(filename);

    Grayscale image_g = (sobelEdgeDetect(image_blur));
    filename = "mario2edgeswithblur.png";
    image_g.exportPNG(filename);

    return 0;
}