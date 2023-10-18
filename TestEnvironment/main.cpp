#include <vector>
#include <time.h>
#include <string>
#include <iostream>
#include <random>
#include <thread>
#include <chrono>
#include <immintrin.h>
#include "Matrix.h"

Matrix<int> MatrixMultiplyAVX2(Matrix<int>& A, Matrix<int>& B) {
    size_t rowsA = A.numRows();
    size_t colsA = A.numCols();
    size_t colsB = B.numCols();
    Matrix<int> C(rowsA, colsA);

    for (size_t i = 0; i < rowsA; ++i) { // iterate over rows 

        for (size_t j = 0; j < colsA; j += 8) { // iterate over cols in blocks of 8

            __m256i sum = _mm256_setzero_si256();

            for (size_t k = 0; k < colsB; k ++) {

                __m256i a = _mm256_set1_epi32(A(i, k));
                __m256i b = _mm256_loadu_si256((__m256i*)&B(k, j));
                __m256i axb = _mm256_mullo_epi32(a, b);
                sum = _mm256_add_epi16(sum, axb);
                //C(i, k) = _mm256_extract_epi32(sum, 0) + _mm256_extract_epi32(sum, 1) + _mm256_extract_epi32(sum, 2) + _mm256_extract_epi32(sum, 3) + _mm256_extract_epi32(sum, 4) + _mm256_extract_epi32(sum, 5) + _mm256_extract_epi32(sum, 6) + _mm256_extract_epi32(sum, 7);
            }
            _mm256_storeu_si256((__m256i*)&C(i,j), sum);
            // Horizontal sum of AVX register
            //for (int c = 0; c < rowsA; c++){
            //    C(i, c) = _mm256_extract_epi32(sum, c);
            //}
            //C(i, k) = _mm256_extract_epi32(sum, 0) + _mm256_extract_epi32(sum, 1) + _mm256_extract_epi32(sum, 2) + _mm256_extract_epi32(sum, 3) + _mm256_extract_epi32(sum, 4) + _mm256_extract_epi32(sum, 5) + _mm256_extract_epi32(sum, 6) + _mm256_extract_epi32(sum, 7);
        
        }
    
    }
    return C;
}

// Matrix<int> MatrixMultiplyAVX2(Matrix<int>& A, Matrix<int>& B) {
//     size_t rowsA = A.numRows();
//     size_t colsA = A.numCols();
//     size_t colsB = B.numCols();
//     Matrix<int> C(rowsA, colsA);

//     printf("%d", rowsA);

//     for (size_t i = 0; i < rowsA; ++i) {
//         for (size_t j = 0; j < colsA; ++j) {
//             auto sum = _mm256_setzero_si256();
//             for (size_t l = 0; l < colsB; l += 8) {
//                 auto a = _mm256_loadu_si256((__m256i*)&A(i, l));
//                 auto b = _mm256_loadu_si256((__m256i*)&B(l, j));
//                 sum += _mm256_madd_epi16(a, b);
//             }
//             // Horizontal sum of AVX register
//             C(i, j) = _mm256_extract_epi32(sum, 0) + _mm256_extract_epi32(sum, 1) + _mm256_extract_epi32(sum, 2) + _mm256_extract_epi32(sum, 3) + _mm256_extract_epi32(sum, 4) + _mm256_extract_epi32(sum, 5) + _mm256_extract_epi32(sum, 6) + _mm256_extract_epi32(sum, 7);
//         }
//     }
//     return C;
// }

// Matrix<int> MatrixMultiplyAVX2(Matrix<int>& A, Matrix<int>& B) {
//     size_t rowsA = A.numRows();
//     size_t colsA = A.numCols();
//     size_t colsB = B.numCols();
//     Matrix<int> C(rowsA, colsA);

//     for (size_t i = 0; i < rowsA; ++i) {
//         for (size_t j = 0; j < colsA; ++j) {
//             auto sum = _mm256_setzero_si256();
//             for (size_t l = 0; l < colsB; l += 8) {
//                 auto a = _mm256_loadu_si256(&A(i, l));
//                 auto b = _mm256_loadu_si256(&B(l, j));
//                 sum = _mm256_fmadd_ps(a, b, sum);
//             }

//             // Horizontal sum of AVX register
//             __m128 hsum = _mm_add_ps(_mm256_extractf128_ps(sum, 0), _mm256_extractf128_ps(sum, 1));

//             // Store the result directly into C(i, j)
//             _mm_storeu_ps(&C(i, j), hsum);
//         }
//     }
//     return C;
// }

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
    Matrix<int> A(8, 8);
    Matrix<int> B(8, 8);
    for (size_t i = 0; i < 8; i++){
        for (size_t j = 0; j < 8; j++){
            A(i, j) = 1;
            B(i, j) = 1;
        }
    }

    // Multiply matrices A and B using SIMD
    Matrix<int> D = MatrixMultiplyAVX2(A, B);
    
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