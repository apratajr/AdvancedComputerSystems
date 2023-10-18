#include <vector>
#include <time.h>
#include <string>
#include <iostream>
#include <random>
#include <thread>
#include <chrono>
#include <immintrin.h>
#include "Matrix.h"

// Matrix<int> MatrixMultiplyAVX2(Matrix<int>& A, Matrix<int>& B) {
//     size_t rowsA = A.numRows();
//     size_t colsA = A.numCols();
//     size_t rowsB = B.numRows();
//     size_t colsB = B.numCols();
//     Matrix<int> C(rowsA, colsB);
//     for (size_t i = 0; i < rowsA; ++i) { // iterate over rows 
//         for (size_t j = 0; j < colsB; j += 8) { // iterate over cols in blocks of 8
//             __m256i sum = _mm256_setzero_si256();
//             for (size_t k = 0; k < colsB; k ++) {
//                 __m256i a = _mm256_set1_epi32(A(i, k));
//                 __m256i b = _mm256_loadu_si256((__m256i*)&B(k, j));
//                 __m256i axb = _mm256_mullo_epi32(a, b);
//                 sum = _mm256_add_epi16(sum, axb);
//             }
//             _mm256_storeu_si256((__m256i*)&C(i,j), sum);
//         }
//     }
//     return C;
// }

// template <typename T>
// Matrix<T> MatrixMultiplyAVX2_Int(Matrix<T>& A, Matrix<T>& B) {
//     // AVX2 Function Comments are from Intel's website at
//     //  https://software.intel.com/sites/landingpage/IntrinsicsGuide/
//     // Note that this code is based on a very useful thread on StackOverflow:
//     //  https://codereview.stackexchange.com/questions/177616/avx-simd-in-matrix-multiplication

//     size_t rowsA = A.numRows();                   // Extract row/col information
//     size_t colsA = A.numCols();
//     size_t rowsB = B.numRows();
//     size_t colsB = B.numCols();
//     Matrix<T> C(rowsA, colsB);                // Initialize result matrix C

//     for (size_t i = 0; i < rowsA; ++i) { // iterate over rows 
//         for (size_t j = 0; j < colsB; j += 8) { // iterate over cols in blocks of 8
//                 __m256i sum = _mm256_setzero_si256();
//             for (size_t k = 0; k < colsB; k ++) {
//                 __m256i a = _mm256_set1_epi32(A(i, k));
//                 __m256i b = _mm256_loadu_si256((__m256i*)&B(k, j));
//                 __m256i axb = _mm256_mullo_epi32(a, b);
//                 sum = _mm256_add_epi32(sum, axb);
//             }
//             _mm256_storeu_si256((__m256i*)&C(i,j), sum);
//         }
//     }
//     return C; // Return result matrix A x B = C
// }

// template <typename T>
// Matrix<T> MatrixMultiplyAVX2_Float(Matrix<T>& A, Matrix<T>& B) {
//     // AVX2 Function Comments are from Intel's website at
//     //  https://software.intel.com/sites/landingpage/IntrinsicsGuide/
//     // Note that this code is based on a very useful thread on StackOverflow:
//     //  https://codereview.stackexchange.com/questions/177616/avx-simd-in-matrix-multiplication

//     size_t rowsA = A.numRows();                   // Extract row/col information
//     size_t colsA = A.numCols();
//     size_t rowsB = B.numRows();
//     size_t colsB = B.numCols();
//     Matrix<T> C(rowsA, colsB);                // Initialize result matrix C

//     for (size_t i = 0; i < rowsA; ++i) {          // Iterate over rows of A
//         for (size_t j = 0; j < colsB; j += 8) {   // Iterate over cols of B in blocks of 8
//             __m256 sum = _mm256_setzero_ps();     // Return vector of type __m256 with all
//             for (size_t k = 0; k < colsB; k++) {  //   elements set to zero.
//                 // Broadcast single-precision (32-bit) floating-point value a to all
//                 //   elements of dst.
//                 __m256 a = _mm256_set1_ps(A(i, k));

//                 // Load 256-bits (composed of 8 packed single-precision (32-bit)
//                 //   floating-point elements) from memory into dst. mem_addr does
//                 //   not need to be aligned on any particular boundary.
//                 __m256 b = _mm256_loadu_ps(&B(k, j)); 

//                 // Multiply packed single-precision (32-bit) floating-point elements
//                 //   in a and b, and store the results in dst.
//                 __m256 axb = _mm256_mul_ps(a, b);

//                 // Add packed single-precision (32-bit) floating-point elements
//                 //   in a and b, and store the results in dst.
//                 sum = _mm256_add_ps(sum, axb);
//             }
//             // Store 256-bits (composed of 8 packed single-precision (32-bit)
//             //   floating-point elements) from a into memory. mem_addr does not
//             //   need to be aligned on any particular boundary.
//             _mm256_storeu_ps(&C(i, j), sum);
//         }
//     }
//     return C; // Return result matrix A x B = C
// }

// Define a template function for matrix multiplication
template <typename T>
Matrix<T> MatrixMultiplyAVX2(Matrix<T>& A, Matrix<T>& B) {
    size_t rowsA = A.numRows();
    size_t colsA = A.numCols();
    size_t rowsB = B.numRows();
    size_t colsB = B.numCols();
    Matrix<T> C(rowsA, colsB);

    for (size_t i = 0; i < rowsA; ++i) {
        for (size_t j = 0; j < colsB; j += 8) {
            auto sum = _mm256_setzero_si256();
            for (size_t k = 0; k < colsB; k++) {
                auto a = _mm256_set1_epi32(A(i, k));
                auto b = _mm256_loadu_si256(reinterpret_cast<__m256i*>(&B(k, j)));
                auto axb = _mm256_mullo_epi32(a, b);
                sum = _mm256_add_epi32(sum, axb);
            }
            _mm256_storeu_si256((__m256i*)(&C(i, j)), sum);        }
    }
    return C;
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

int main() {
    // Create two matrices A and B with float elements
    typedef int r;
    unsigned int num = 8;
    Matrix<r> A(num, num);
    Matrix<r> B(num, num);
    for (size_t i = 0; i < num; i++){
        for (size_t j = 0; j < num; j++){
            A(i, j) = 1;
            B(i, j) = 1;
        }
    }
    typedef float s;
    Matrix<s> D(num, num);
    Matrix<s> E(num, num);
    for (size_t i = 0; i < num; i++){
        for (size_t j = 0; j < num; j++){
            D(i, j) = 1.5;
            E(i, j) = 1.5;
        }
    }
    // // Display A
    // A.print();
    // printf("\n\n");
    // // Display B
    // B.print();
    // printf("\n\n");
    // Multiply matrices A and B using SIMD
    Matrix<r> C = MatrixMultiplyAVX2(A, B);
    Matrix<s> F = MatrixMultiplyAVX2(D, E);
    // Display A
    A.print();
    printf("\n\n");
    // Display B
    B.print();
    printf("\n\n");
    // Display the result matrix C
    D.print();
    printf("\n\n");

    return 0;
}