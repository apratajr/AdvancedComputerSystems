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
Grayscale gaussianBlur(const Grayscale& input);
Grayscale gaussianBlurStrong(const Grayscale& input);
Grayscale sobelEdgeDetect(const Grayscale& input);
double** generateGaussianKernel(int radius, double stddev);
void destroyGaussianKernel(double** kernel, int radius);
