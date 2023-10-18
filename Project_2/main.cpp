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
#include <thread>
#include <chrono>
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

// Function to automatically populate matrix A with random Integers
template <typename T>
void populateRandomInteger(Matrix<T>& A) {
    unsigned int minValue = 0;
    unsigned int maxValue = 9;
    std::random_device rd;  // Seed for the random number generator
    std::mt19937 gen(rd()); // Mersenne Twister PRNG
    std::uniform_int_distribution<T> dist(minValue, maxValue); /// CURRENTLY FIXED PT ONLY ///
    for (size_t i = 0; i < A.numRows(); ++i) {
        for (size_t j = 0; j < A.numCols(); ++j) {
            A(i, j) = dist(gen);
        }
    }  
}

// Function to automatically populate matrix A with random Integers
template <typename T>
void populateRandomFloat(Matrix<T>& A) {
    float minValue = 0.0f;
    float maxValue = 9.9f;
    std::random_device rd;  // Seed for the random number generator
    std::mt19937 gen(rd()); // Mersenne Twister PRNG
    std::uniform_real_distribution<T> dist(minValue, maxValue); /// CURRENTLY FIXED PT ONLY ///
    for (size_t i = 0; i < A.numRows(); ++i) {
        for (size_t j = 0; j < A.numCols(); ++j) {
            A(i, j) = dist(gen);
        }
    }  
}

// Function to execute the multiplication testing
template <typename T>
void testExecute(Matrix<T>& A, Matrix<T>& B) {
    // Compute the product A x B = C

    if (multiThreading) {
        printf("\r\n\n\tComputing product A x B = C now ... ");
        auto startMultiply = std::chrono::high_resolution_clock::now();
        Matrix<T> result = multiplyMatricesMultithreaded(A, B);
        auto stopMultiply = std::chrono::high_resolution_clock::now();
        auto durationMultiply = std::chrono::duration_cast<std::chrono::microseconds>
            (stopMultiply - startMultiply);
        printf("\r\n\n\tMatrices multiplied. Elapsed time: %.6f seconds.",
            static_cast<double>(durationMultiply.count()) / 1000000);
        printf("\r\n\n\t");
    }
    else {
        printf("\r\n\n\tComputing product A x B = C now ... ");
        auto startMultiply = std::chrono::high_resolution_clock::now();
        Matrix<T> result = naiveMultiply(A, B);
        auto stopMultiply = std::chrono::high_resolution_clock::now();
        auto durationMultiply = std::chrono::duration_cast<std::chrono::microseconds>
            (stopMultiply - startMultiply);
        printf("\r\n\n\tMatrices multiplied. Elapsed time: %.6f seconds.",
            static_cast<double>(durationMultiply.count()) / 1000000);
        printf("\r\n\n\t");
    } 
}

main(int argc, char* argv[]) {
    if (argc < 6) {
        std::cerr << "Usage: " << argv[0] << " <multithreading [1/0]> <simd [1/0]> "
            "<cache optimization [1/0]> <matrix type [int/float]> <matrix size [100-10000]>" << std::endl;
        return 1; // Return an error code
    }
    // Assign command line parameters
    multiThreading = std::stoi(argv[1]);
    SIMD = std::stoi(argv[2]);
    cacheOptimization = std::stoi(argv[3]);
    if (std::string(argv[4]) == "float") {
        float_ = true;
    }
    rows_ = cols_ = std::stoi(argv[5]);

    // Print configuration information for testing
    printf("\r\n\tMatrix Multiplication Optimization Testing\r\n\tProgram Version 10.17.0"
           "\r\n\t__________________________________________");
    printf("\r\n\n\tProceeding with settings\r\n\t multiThreading = %s",
        multiThreading ? "true" : "false");
    printf("\r\n\t SIMD = %s",
        SIMD ? "true" : "false");
    printf("\r\n\t cacheOptimization = %s",
        cacheOptimization ? "true" : "false");
    printf("\r\n\t matrix type = %s", argv[4]);
    printf("\r\n\t matrix size = %d x %d", rows_, cols_);

    // Generate and populate row_ x col_ matrices for testing
    printf("\r\n\n\tGenerating random %d x %d matrices A and B ...", rows_, cols_);
    auto startPopulate = std::chrono::high_resolution_clock::now();
    // Floating point matrices being used
    if (float_) {
        Matrix<float> A(rows_, cols_);
        Matrix<float> B(rows_, cols_);
        populateRandomFloat(A);
        populateRandomFloat(B);
        auto stopPopulate = std::chrono::high_resolution_clock::now();
        auto durationPopulate = std::chrono::duration_cast<std::chrono::microseconds>
            (stopPopulate - startPopulate);
        printf("\r\n\n\tMatrices populated. Elapsed time: %.6f seconds.",
            static_cast<double>(durationPopulate.count()) / 1000000);
        testExecute(A, B);
    }
    // Integer matrices being used
    else {
        Matrix<int> A(rows_, cols_);
        Matrix<int> B(rows_, cols_);
        populateRandomInteger(A);
        populateRandomInteger(B);
        auto stopPopulate = std::chrono::high_resolution_clock::now();
        auto durationPopulate = std::chrono::duration_cast<std::chrono::microseconds>
            (stopPopulate - startPopulate);
        printf("\r\n\n\tMatrices populated. Elapsed time: %.6f seconds.",
            static_cast<double>(durationPopulate.count()) / 1000000);
        testExecute(A, B);
    }

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