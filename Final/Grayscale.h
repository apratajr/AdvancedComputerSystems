//
// file: Grayscale.h
// desc: ACS Final Project Class Header
// auth: Andrew Prata
//
// This program implements a grayscale image class,
// as well as necessary helper functions.
//

#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>
#include "lodepng.h" // https://github.com/lvandeve/lodepng/tree/master

class Grayscale {
private:
    unsigned width;
    unsigned height;
    unsigned char* data; // Use unsigned char for pixel values (0-255)

public:
    Grayscale(int w, int h);
    Grayscale(const std::string& filename);
    ~Grayscale();

    void exportPNG(const std::string& filename);
    void setPixel(unsigned x, unsigned y, unsigned char value);
    unsigned char getPixel(unsigned x, unsigned y) const;
    unsigned getWidth() const;
    unsigned getHeight() const;
};

Grayscale invert(const Grayscale& input);
Grayscale gaussianBlur(const Grayscale& input, const int kernelSize);
Grayscale meanBlur(const Grayscale& input);
Grayscale sobelEdgeDetect(const Grayscale& input);
Grayscale contour(const Grayscale& input);
Grayscale detectCorners(const Grayscale& input, double threshold);
Grayscale convolution(const Grayscale& input, const std::vector<std::vector<int>>& kernel);
Grayscale histogramEqualization(Grayscale& input);
Grayscale medianFilter(const Grayscale& input, int windowSize);
