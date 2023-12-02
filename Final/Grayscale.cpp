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
Grayscale gaussianBlur(const Grayscale& input, const int kernelSize) {
    unsigned width = input.getWidth();
    unsigned height = input.getHeight();
    Grayscale output(width, height);

    // Choose the Gaussian blur kernel based on the provided size
    std::vector<std::vector<int>> gaussianKernel;
    int kernelRadius;
    switch (kernelSize) {
        case 3:
            gaussianKernel = {{1, 2, 1},
                              {2, 4, 2},
                              {1, 2, 1}};
            kernelRadius = 1;
            break;
        case 5:
            gaussianKernel = {{1, 4,  7,  4,  1},
                              {4, 16, 26, 16, 4},
                              {7, 26, 41, 26, 7},
                              {4, 16, 26, 16, 4},
                              {1, 4,  7,  4,  1}};
            kernelRadius = 2;
            break;
        case 7:
            gaussianKernel = {{0, 0,  1,  2,  1,   0,  0},
                              {0, 3,  13, 22,  13, 3,  0},
                              {1, 13, 59, 97,  59, 13, 1},
                              {2, 22, 97, 159, 97, 22, 2},
                              {1, 13, 59, 97,  59, 13, 1},
                              {0, 3,  13, 22,  13, 3,  0},
                              {0, 0,  1,  2,   1,  0,  0}};
            kernelRadius = 3;
            break;
        default:
            std::cerr << "Unsupported kernel size for Gaussian blur." << std::endl;
            std::exit(EXIT_FAILURE);
    }

    // Iterate over the input image, excluding edges (kernel would overlap image)
    for (unsigned y = kernelRadius; y < height - kernelRadius; ++y) {
        for (unsigned x = kernelRadius; x < width - kernelRadius; ++x) {
            unsigned sum = 0;
            unsigned weightSum = 0;

            // Perform convolution (input * Gaussian kernel)
            for (int a = -kernelRadius; a <= kernelRadius; ++a) {
                for (int b = -kernelRadius; b <= kernelRadius; ++b) {
                    unsigned char input_pixel = input.getPixel(x + a, y + b);
                    sum += gaussianKernel[a + kernelRadius][b + kernelRadius] * input_pixel;
                    weightSum += gaussianKernel[a + kernelRadius][b + kernelRadius];
                }
            }
            output.setPixel(x, y, static_cast<unsigned char>(sum / weightSum));
        }
    }

    return output;
}

// Classic mean blur kernel convolution
Grayscale meanBlur(const Grayscale& input) {
    unsigned width = input.getWidth();
    unsigned height = input.getHeight();
    Grayscale output(width, height);
    // Mean blur kernel (AVERAGE!)
    int mean[5][5] = {{1, 1, 1, 1, 1}, {1, 1, 1, 1, 1}, {1, 1, 1, 1, 1}, {1, 1, 1, 1, 1}, {1, 1, 1, 1, 1}};
    // Iterate over the input image, excluding edges (kernel would overlap image)
    for (unsigned y = 2; y < height - 2; ++y) {
        for (unsigned x = 2; x < width - 2; ++x) {
            int sum = 0;
            for (unsigned a = 0; a < 5; ++a) {
                for (unsigned b = 0; b < 5; ++b) {
                    unsigned char input_pixel = input.getPixel(x + a - 2, y + b - 2);
                    sum += mean[a][b] * input_pixel;
                }
            }
            sum /= 25;
            output.setPixel(x, y, sum);
        }
    }
    return output;
}

// Contour/edge function
Grayscale contour(const Grayscale& input) {
    unsigned width = input.getWidth();
    unsigned height = input.getHeight();
    Grayscale output(width, height);
    // Mean blur kernel (AVERAGE!)
    int outline[3][3] = {{-1, -1, -1}, {-1, 8, -1}, {-1, -1, -1}};
    // Iterate over the input image, excluding edges (kernel would overlap image)
    for (unsigned y = 1; y < height - 1; ++y) {
        for (unsigned x = 1; x < width - 1; ++x) {
            int sum = 0;
            for (unsigned a = 0; a < 3; ++a) {
                for (unsigned b = 0; b < 3; ++b) {
                    unsigned char input_pixel = input.getPixel(x + a - 1, y + b - 1);
                    sum += outline[a][b] * input_pixel;
                }
            }
            output.setPixel(x, y, sum);
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

            output.setPixel(x, y, (mag > 64) ? (int)mag : 0);
            //output.setPixel(x, y, (int)mag);
        }
    }
    return output;
}

Grayscale convolution(const Grayscale& input, const std::vector<std::vector<int>>& kernel) {
    unsigned width = input.getWidth();
    unsigned height = input.getHeight();
    Grayscale output(width, height);

    // Iterate over the input image, excluding edges (kernel would overlap image)
    for (unsigned y = kernel.size() / 2; y < height - kernel.size() / 2; ++y) {
        for (unsigned x = kernel[0].size() / 2; x < width - kernel[0].size() / 2; ++x) {
            int sum = 0;

            // Perform convolution (input * kernel)
            for (int a = -kernel.size() / 2; a <= kernel.size() / 2; ++a) {
                for (int b = -kernel[0].size() / 2; b <= kernel[0].size() / 2; ++b) {
                    unsigned char input_pixel = input.getPixel(x + a, y + b);
                    sum += kernel[a + kernel.size() / 2][b + kernel[0].size() / 2] * input_pixel;
                }
            }
            output.setPixel(x, y, static_cast<unsigned char>(std::abs(sum)));
        }
    }
    return output;
}

// Function to detect corners using the Harris Corner Detector with your Grayscale class
Grayscale detectCorners(const Grayscale& inputImage, double threshold) {
    unsigned width = inputImage.getWidth();
    unsigned height = inputImage.getHeight();

    Grayscale inputGray(width, height);

    // Convert inputImage to grayscale
    for (unsigned y = 0; y < height; ++y) {
        for (unsigned x = 0; x < width; ++x) {
            unsigned char pixelValue = inputImage.getPixel(x, y);
            inputGray.setPixel(x, y, pixelValue);
        }
    }

    Grayscale outputImage(width, height);
    Grayscale corners(width, height);

    // Harris Corner Detector
    for (unsigned y = 1; y < height - 1; ++y) {
        for (unsigned x = 1; x < width - 1; ++x) {
            // Compute gradients
            int dx = inputGray.getPixel(x + 1, y) - inputGray.getPixel(x - 1, y);
            int dy = inputGray.getPixel(x, y + 1) - inputGray.getPixel(x, y - 1);

            // Harris Corner Response
            int cornerResponse = std::abs(dx * dy - 0.04 * (dx + dy) * (dx + dy));

            corners.setPixel(x, y, static_cast<unsigned char>(cornerResponse));
        }
    }

    // Normalize the corner response to [0, 255]
    unsigned maxResponse = 0;
    for (unsigned y = 0; y < height; ++y) {
        for (unsigned x = 0; x < width; ++x) {
            maxResponse = std::max(maxResponse, static_cast<unsigned>(corners.getPixel(x, y)));
        }
    }

    for (unsigned y = 0; y < height; ++y) {
        for (unsigned x = 0; x < width; ++x) {
            unsigned char normalizedResponse = static_cast<unsigned char>(
                255 * static_cast<double>(corners.getPixel(x, y)) / static_cast<double>(maxResponse));

            outputImage.setPixel(x, y, normalizedResponse);
        }
    }
    // Highlight corners by thresholding
    for (unsigned y = 0; y < height; ++y) {
        for (unsigned x = 0; x < width; ++x) {
            if (corners.getPixel(x, y) > threshold) {
                outputImage.setPixel(x, y, 255);  // White pixel for corners
            } else {
                outputImage.setPixel(x, y, 0);    // Black pixel otherwise
            }
        }
    }
    return outputImage;
}