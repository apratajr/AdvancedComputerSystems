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

    // Create a new Grayscale object that performs an operation on the input
    Grayscale image_blur_strong = gaussianBlurStrong(input_image);

    // Create a new Grayscale object that performs an operation on the input
    Grayscale image_edges = (sobelEdgeDetect(image_blur_strong));

    // Export the final processed image to the output
    image_edges.exportPNG((std::string)argv[2]);

    int radius = 2;
    double** kernel = generateGaussianKernel(radius, 1.0);

    for (int i = 0; i < 2*radius+1; i++) {
        for (int j = 0; j < 2*radius+1; j++) {
            std::cout << kernel[i][j] << " ";
        }
        std::cout << std::endl;
    }

    destroyGaussianKernel(kernel, radius);

    return 0;
}