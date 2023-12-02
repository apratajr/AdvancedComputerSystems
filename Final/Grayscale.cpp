//
// file: Grayscale.cpp
// desc: ACS Final Project Class Implementation
// auth: Andrew Prata
//
// This program implements a grayscale image class,
// as well as necessary helper functions.
//

#include "Grayscale.h"

// Constructor given W, H
Grayscale::Grayscale(int w, int h) : width(w), height(h) {
    // Allocate memory for the image data
    data = new unsigned char[width * height];
}

// Constructor given an existing PNG path
Grayscale::Grayscale(const std::string& filename) {
    std::vector<unsigned char> png;   // Entire PNG image (in PNG format)
    std::vector<unsigned char> image; // Actual image (raw pixels)

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
Grayscale::~Grayscale() {
    // Deallocate memory when the object is destroyed
    delete[] data;
}

// Export a Grayscale object to PNG given a filename
void Grayscale::exportPNG(const std::string& filename) {
    // Create vector to hold PNG image
    std::vector<unsigned char> png;

    // Convert grayscale data to RGBA format (PNG uses 4 channels: Red, Green, Blue, Alpha)
    std::vector<unsigned char> imageRGBA;
    for (size_t i = 0; i < width * height; ++i) {
        // Assign the same grayscale value to Red, Green, Blue, and set Alpha to 255 (fully opaque)
        imageRGBA.push_back(data[i]); // PNG R
        imageRGBA.push_back(data[i]); // PNG G
        imageRGBA.push_back(data[i]); // PNG B
        imageRGBA.push_back(255);     // PNG A
    }

    // Encode the image to PNG format
    unsigned error = lodepng::encode(png, imageRGBA, width, height);

    if (error) {
        std::cerr << "Error encoding PNG file: " << lodepng_error_text(error) << std::endl;
        std::exit(EXIT_FAILURE);
    }

    // Write the PNG data to a file
    error = lodepng::save_file(png, filename);

    if (error) {
        std::cerr << "Error saving PNG file: " << lodepng_error_text(error) << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

// Set the pixel value at coordinates (x, y)
void Grayscale::setPixel(unsigned x, unsigned y, unsigned char value) {
    if (x < width && y < height) {
        data[y * width + x] = value;
    } else {
        std::cerr << "Invalid pixel coordinates." << std::endl;
    }
}

// Get the pixel value at coordinates (x, y)
unsigned char Grayscale::getPixel(unsigned x, unsigned y) const {
    if (x < width && y < height) {
        return data[y * width + x];
    } else {
        std::cerr << "Invalid pixel coordinates." << std::endl;
        return 0; // Return 0 for out-of-bounds access
    }
}

// Get the width of the image
unsigned Grayscale::getWidth() const {
    return width;
}

// Get the height of the image
unsigned Grayscale::getHeight() const {
    return height;
}

// Image inverter (created for testing I/O)
Grayscale invert(const Grayscale& input) {
    unsigned width = input.getWidth();
    unsigned height = input.getHeight();
    Grayscale output(width, height);
    for (unsigned y = 0; y < height; ++y) {
        for (unsigned x = 0; x < width; ++x) {
            unsigned char originalValue = input.getPixel(x, y);
            unsigned char invertedValue = 255 - originalValue;
            output.setPixel(x, y, invertedValue);
        }
    }
    return output;
}

// Apply Gaussian Blur
Grayscale gaussianBlur(const Grayscale& input) {
    unsigned width = input.getWidth();
    unsigned height = input.getHeight();
    Grayscale output(width, height);
    // Gaussian blur kernel
    unsigned gaussian[3][3] = {{1, 2, 1}, {2, 4, 2}, {1, 2, 1}};
    // Iterate over the input image, excluding edges (kernel would overlap image)
    for (unsigned y = 1; y < height - 1; ++y) {
        for (unsigned x = 1; x < width - 1; ++x) {
            unsigned sum = 0;

            // Perform convolution (input * sorbel kernels)
            for (unsigned a = 0; a < 3; ++a) {
                for (unsigned b = 0; b < 3; ++b) {
                    unsigned char input_pixel = input.getPixel(x + a - 1, y + b - 1);
                    sum += gaussian[a][b] * input_pixel;
                }
            }
            output.setPixel(x, y, (int)sum/16);
        }
    }
    return output;
}

Grayscale gaussianBlurStrong(const Grayscale& input) {
    unsigned width = input.getWidth();
    unsigned height = input.getHeight();
    Grayscale output(width, height);
    // Gaussian blur kernel
    int gaussian[5][5] = {{1, 4, 6, 4, 1},
                          {4, 16, 24, 16, 4},
                          {6, 24, 36, 24, 6},
                          {4, 16, 24, 16, 4},
                          {1, 4, 6, 4, 1}};
    // Iterate over the input image, excluding edges (kernel would overlap image)
    for (unsigned y = 2; y < height - 2; ++y) {
        for (unsigned x = 2; x < width - 2; ++x) {
            unsigned sum = 0;
            unsigned weightSum = 0;

            // Perform convolution (input * Gaussian kernel)
            for (int a = -2; a <= 2; ++a) {
                for (int b = -2; b <= 2; ++b) {
                    unsigned char input_pixel = input.getPixel(x + a, y + b);
                    sum += gaussian[a + 2][b + 2] * input_pixel;
                    weightSum += gaussian[a + 2][b + 2];
                }
            }
            output.setPixel(x, y, static_cast<unsigned char>(sum / weightSum));
        }
    }
    return output;
}

// Apply the Sobel Operator
Grayscale sobelEdgeDetect(const Grayscale& input) {
    unsigned width = input.getWidth();
    unsigned height = input.getHeight();
    Grayscale output(width, height);
    // Sobel operator kernels
    int sobel_x[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int sobel_y[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};
    // Iterate over the input image, excluding edges (kernel would overlap image)
    for (unsigned y = 1; y < height - 1; ++y) {
        for (unsigned x = 1; x < width - 1; ++x) {
            int sum_x = 0;
            int sum_y = 0;

            // Perform convolution (input * sorbel kernels)
            for (unsigned a = 0; a < 3; ++a) {
                for (unsigned b = 0; b < 3; ++b) {
                    unsigned char input_pixel = input.getPixel(x + a - 1, y + b - 1);
                    sum_x += sobel_x[a][b] * input_pixel;
                    sum_y += sobel_y[a][b] * input_pixel;
                }
            }
            
            // Magnitude of convolution result (Sobel gradient approximation)
            int mag = (int)sqrt(sum_x * sum_x + sum_y * sum_y);

            // output.setPixel(x, y, (mag > 32) ? 255 : 0);
            output.setPixel(x, y, (int)mag);
        }
    }
    return output;
}

double** generateGaussianKernel(int radius, double stddev) {
    // Calculate the size of the kernel
    int size = 2 * radius + 1;

    // Allocate memory for the kernel
    double** kernel = new double*[size];
    for (int i = 0; i < size; ++i) {
        kernel[i] = new double[size];
    }

    // Calculate the values for the Gaussian kernel
    double sum = 0;
    for (int i = -radius; i <= radius; ++i) {
        for (int j = -radius; j <= radius; ++j) {
            double value = exp(-(i * i + j * j) / (2 * stddev * stddev));
            value /= (2 * M_PI * stddev * stddev);
            kernel[i + radius][j + radius] = value;
            sum += value;
        }
    }
    for (int x = 0; x < size; x++) {
        for (int y = 0; y < size; y++) {
            kernel[x][y] /= sum;
        }
    }

    return kernel;
}

void destroyGaussianKernel(double** kernel, int radius) {
    int size = 2 * radius + 1;
    for (int i = 0; i < size; ++i) {
        delete[] kernel[i];
    }
    delete[] kernel;
}