//
// file: main.cpp
// desc: ACS Final Project MAIN
// auth: Andrew Prata
//
// This program implements basic image processing
// functionality, emphasizing edge-detection.
//

#include "Grayscale.h" // Image data structure

int main(int argc, char* argv[]) {
    if (argc < 4) { // Ensure correct commandline arguments
        std::cerr << "Usage: " << argv[0] << " <path/to/input_image.png>"
            " <path/to/output_image.png> <operation [0-9]>" << std::endl;
        return 1;   // Return an error code
    }

    // Create a Grayscale object from input PNG
    Grayscale input_image((std::string)argv[1]);

    Grayscale median_image = medianFilter(input_image, 2);

    Grayscale equalized = histogramEqualization(median_image);

    equalized.exportPNG((std::string)argv[2]);

    return 0;
}