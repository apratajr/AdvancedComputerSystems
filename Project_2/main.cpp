//
// file: main.cpp
// desc: ACS Project 2 MAIN
// auth: Andrew Prata
//
// This program performs matrix-matrix multiplication.
// It includes three optimization options for this
// operation. These are: multithreading, SIMD, and
// cache usage optimization.
//

// Includes
#include <vector>
#include <time.h>
#include <string>
#include <iostream>
#include <random>
#include "Matrix.h"

// Global optimization flags
bool multiThreading    = false;
bool SIMD              = false;
bool cacheOptimization = false;
unsigned int threads   = 1;

// Matrix testing variables (default lowest matrix size)
unsigned int rows_ = 100;
unsigned int cols_ = 100;
bool float_        = false;

// Function to automatically populate array A with random values
template <typename T>
void populateRandom(Matrix<T>& A) {
    unsigned int minValue = 0;
    unsigned int maxValue = 10;
    std::random_device rd;  // Seed for the random number generator
    std::mt19937 gen(rd()); // Mersenne Twister PRNG
    std::uniform_int_distribution<T> dist(minValue, maxValue); /// CURRENTLY FIXED PT ONLY ///

    for (size_t i = 0; i < A.numRows(); ++i) {
        for (size_t j = 0; j < A.numCols(); ++j) {
            A(i, j) = dist(gen);
        }
    }  
}

main() {
    std::string temp;
    printf("\r\n\tMatrix Multiplication Optimization Testing\r\n\n");
    printf("\t -> Would you like to use multithreading? [y/n] ");
    std::cin >> temp;
    if (temp == "y") {
        multiThreading = true;
    }
    printf("\r\n\t -> Would you like to use SIMD? [y/n] ");
    std::cin >> temp;
    if (temp == "y") {
        SIMD = true;
    }
    printf("\r\n\t -> Would you like to use cache access optimization? [y/n] ");
    std::cin >> temp;
    if (temp == "y") {
        cacheOptimization = true;
    }
    printf("\r\n\tProceeding with settings\r\n\t multiThreading = %s",
        multiThreading ? "true" : "false");
    printf("\r\n\t SIMD = %s",
        SIMD ? "true" : "false");
    printf("\r\n\t cacheOptimization = %s",
        cacheOptimization ? "true" : "false");
    printf("\r\n\n\tMatrix Size Selection");
    printf("\r\n\n\t [1] 100 x 100");
    printf("\r\n\t [2] 1,000 x 1,000");
    printf("\r\n\t [3] 10,000 x 10,000");
    printf("\r\n\n\t -> What size to use for test matrices? [1/2/3] ");
    std::cin >> temp;
    if (temp == "2") {
        rows_ = cols_ = 1000;
    }
    else if (temp == "3") {
        rows_ = cols_ = 10000;
    }
    printf("\r\n\n\tGenerating random %d x %d matrices A and B...\r\n\n", rows_, cols_);
    Matrix<int> A(rows_, cols_);
    Matrix<int> B(3, 3);
    populateRandom(A);
    populateRandom(B);

    B.print();

    // // General testing stuff
    // Matrix<int> intMatrix(3, 3);
    
    // for (int i = 0; i < intMatrix.numRows(); ++i) {
    //     for (int j = 0; j < intMatrix.numCols(); ++j) {
    //         intMatrix(i, j) = i * intMatrix.numCols() + j;
    //     }
    // }

    // intMatrix.print();
    // printf("\n");

    // Matrix<int> result = naiveMultiply(intMatrix, intMatrix);

    // result.print();
    // printf("\n");

    // intMatrix.print();

    return 0;
}