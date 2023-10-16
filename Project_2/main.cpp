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

#include <vector>
#include <time.h>
#include <string>
#include <iostream>
#include "Matrix.h"

main(){
    
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