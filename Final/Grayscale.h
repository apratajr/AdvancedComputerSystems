//
// file: Grayscale.h
// desc: ACS Final Project Class Header
// auth: Andrew Prata
//
// This program implements a grayscale image class,
// as well as necessary helper functions.
//

#include <string>
#include <vector>
#include <iostream>
#include <cmath>
#include "lodepng.h"

class Grayscale {
private:
    unsigned width;
    unsigned height;
    unsigned char* data; // Use unsigned char for pixel values (0-255)

public:
    // Constructor given W, H
    Grayscale(int w, int h) : width(w), height(h) {
        // Allocate memory for the image data
        data = new unsigned char[width * height];
    }
    // Construction given an existing PNG path
    Grayscale(const std::string& filename) {
        std::vector<unsigned char> png;
        std::vector<unsigned char> image; //the raw pixels

        // Load and decode
        unsigned error = lodepng::load_file(png, filename);
        if(!error) error = lodepng::decode(image, width, height, png);

        if (error) {
            std::cerr << "Error loading PNG file: " << lodepng_error_text(error) << std::endl;
            std::exit(EXIT_FAILURE);
        }

        // Allocate memory for the image data
        data = new unsigned char[width * height];

        // Convert RGBA input image (from lodepng) to grayscale and store in data
        for (size_t i = 0; i < image.size(); i += 4) {
            // RGB->Grayscale formula from International Telecom Union Rec.709
            // Gray = 0.2126 * Red + 0.7152 * Green + 0.0722 * Blue
            data[i / 4] = static_cast<unsigned char>(std::round(
                0.2126 * image[i] + 0.7152 * image[i + 1] + 0.0722 * image[i + 2]
            )); // Done because data is stored as 4 bytes RGBA RGBA RGBA ...
        }
    }

    // Destructor
    ~Grayscale() {
        // Deallocate memory when the object is destroyed
        delete[] data;
    }

    // Set the pixel value at coordinates (x, y)
    void setPixel(unsigned x, unsigned y, unsigned char value) {
        if (x < width && y < height) {
            data[y * width + x] = value;
        } else {
            std::cerr << "Invalid pixel coordinates." << std::endl;
        }
    }

    // Get the pixel value at coordinates (x, y)
    unsigned char getPixel(unsigned x, unsigned y) const {
        if (x < width && y < height) {
            return data[y * width + x];
        } else {
            std::cerr << "Invalid pixel coordinates." << std::endl;
            return 0; // Return 0 for out-of-bounds access
        }
    }

    void exportPNG(const std::string filename) {
        // Implementation here...
    }

    // Get the width of the image
    unsigned getWidth() const {
        return width;
    }

    // Get the height of the image
    unsigned getHeight() const {
        return height;
    }
};
