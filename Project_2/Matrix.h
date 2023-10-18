//
// file: Matrix.h
// desc: ACS Project 2 Matrix Class Header
// auth: Andrew Prata
//
// This header file contains the Matrix class
// and the accompanying helpers.
//

#define size_t unsigned int // Matrix indices = nonnegative integers

// Matrix class for compact storing and accessing
template <typename T>
class Matrix {
private:
    std::vector<std::vector<T>> data;
    size_t rows;
    size_t cols;

public:
    // Constructor
    Matrix(size_t rows, size_t cols) : rows(rows), cols(cols) {
        data.resize(rows, std::vector<T>(cols)); // Allocate only necessary area
    }

    // Accessor to MODIFY the element at a specific row and column
    T& operator()(size_t row, size_t col) {
        if ((row < rows) && (col < cols)) {
            return data[row][col];
        } else {
            throw std::out_of_range("Matrix indices out of range");
        }
    }

    // Getter methods for rows and columns
    size_t numRows() const {
        return rows;
    }

    size_t numCols() const {
        return cols;
    }

    // Print the matrix
    void print() {
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                std::cout << data[i][j] << ' ';
            }
            std::cout << '\n';
        }
    }
};

// Function to perform "naive" matrix-matrix multiplication
template <typename T>
Matrix<T> naiveMultiply(Matrix<T>& A, Matrix<T>& B) {
    size_t rowsA = A.numRows();
    size_t colsA = A.numCols();
    size_t colsB = B.numCols();

    if (colsA != B.numRows()) {
        throw std::invalid_argument("Matrix dimensions are incompatible for multiplication");
    }

    Matrix<T> result(rowsA, colsB);

    for (size_t i = 0; i < rowsA; ++i) {
        // printf("row: %d\r", i);
        for (size_t j = 0; j < colsB; ++j) {
            T sum = 0;
            for (size_t k = 0; k < colsA; ++k) {
                sum += A(i, k) * B(k, j);
            }
            result(i, j) = sum;
        }
    }

    return result;
}

// Function to multiply a specific portion of the matrices
template <typename T>
void multiplyPartial(
    Matrix<T>& A, Matrix<T>& B, Matrix<T>& C, size_t startRow, size_t endRow) {
    for (size_t i = startRow; i < endRow; ++i) {
        for (size_t j = 0; j < B.numCols(); ++j) {
            C(i, j) = 0;
            for (size_t k = 0; k < A.numCols(); ++k) {
                C(i, j) += A(i, k) * B(k, j);
            }
        }
    }
}

// Function to perform matrix multiplication using multithreading
template <typename T>
Matrix<T> multiplyMatricesMultithreaded(Matrix<T>& A, Matrix<T>& B) {
    if (A.numCols() != B.numRows()) {
        throw std::invalid_argument("Matrix dimensions are not compatible for multiplication");
    }

    size_t numRowsA = A.numRows();
    size_t numRowsB = B.numRows();
    size_t numColsB = B.numCols();

    // Create a result matrix of appropriate size
    Matrix<T> result(numRowsA, numColsB);

    // Determine the number of threads to use (you can adjust this as needed)
    size_t numThreads = std::thread::hardware_concurrency();

    std::vector<std::thread> threads;
    for (size_t threadID = 0; threadID < numThreads; ++threadID) {
        size_t startRow = (threadID * numRowsA) / numThreads;
        size_t endRow = ((threadID + 1) * numRowsA) / numThreads;
        threads.emplace_back(multiplyPartial<T>, std::ref(A), std::ref(B), std::ref(result), startRow, endRow);
    }

    // Join all the threads
    for (auto& thread : threads) {
        thread.join();
    }

    return result;
}

Matrix<int> MatrixMultiplyAVX2_Int(Matrix<int>& A, Matrix<int>& B) {
    // AVX2 Function Comments are from Intel's website at
    //  https://software.intel.com/sites/landingpage/IntrinsicsGuide/
    // Note that this code is based on a very useful thread on StackOverflow:
    //  https://codereview.stackexchange.com/questions/177616/avx-simd-in-matrix-multiplication

    size_t rowsA = A.numRows();                   // Extract row/col information
    size_t colsA = A.numCols();
    size_t rowsB = B.numRows();
    size_t colsB = B.numCols();
    Matrix<int> C(rowsA, colsB);                // Initialize result matrix C

    for (size_t i = 0; i < rowsA; ++i) { // iterate over rows 
        for (size_t j = 0; j < colsB; j += 8) { // iterate over cols in blocks of 8
                __m256i sum = _mm256_setzero_si256();
            for (size_t k = 0; k < colsB; k ++) {
                __m256i a = _mm256_set1_epi32(A(i, k));
                __m256i b = _mm256_loadu_si256((__m256i*)&B(k, j));
                __m256i axb = _mm256_mullo_epi32(a, b);
                sum = _mm256_add_epi32(sum, axb);
            }
            _mm256_storeu_si256((__m256i*)&C(i,j), sum);
        }
    }
    return C; // Return result matrix A x B = C
}

Matrix<float> MatrixMultiplyAVX2_Float(Matrix<float>& A, Matrix<float>& B) {
    // AVX2 Function Comments are from Intel's website at
    //  https://software.intel.com/sites/landingpage/IntrinsicsGuide/
    // Note that this code is based on a very useful thread on StackOverflow:
    //  https://codereview.stackexchange.com/questions/177616/avx-simd-in-matrix-multiplication

    size_t rowsA = A.numRows();               // Extract row/col information
    size_t colsA = A.numCols();
    size_t rowsB = B.numRows();
    size_t colsB = B.numCols();
    Matrix<float> C(rowsA, colsB);                // Initialize result matrix C

    for (size_t i = 0; i < rowsA; ++i) {          // Iterate over rows of A
        for (size_t j = 0; j < colsB; j += 8) {   // Iterate over cols of B in blocks of 8
            __m256 sum = _mm256_setzero_ps();     // Return vector of type __m256 with all
            for (size_t k = 0; k < colsB; k++) {  //   elements set to zero.
                // Broadcast single-precision (32-bit) floating-point value a to all
                //   elements of dst.
                __m256 a = _mm256_set1_ps(A(i, k));

                // Load 256-bits (composed of 8 packed single-precision (32-bit)
                //   floating-point elements) from memory into dst. mem_addr does
                //   not need to be aligned on any particular boundary.
                __m256 b = _mm256_loadu_ps(&B(k, j)); 

                // Multiply packed single-precision (32-bit) floating-point elements
                //   in a and b, and store the results in dst.
                __m256 axb = _mm256_mul_ps(a, b);

                // Add packed single-precision (32-bit) floating-point elements
                //   in a and b, and store the results in dst.
                sum = _mm256_add_ps(sum, axb);
            }
            // Store 256-bits (composed of 8 packed single-precision (32-bit)
            //   floating-point elements) from a into memory. mem_addr does not
            //   need to be aligned on any particular boundary.
            _mm256_storeu_ps(&C(i, j), sum);
        }
    }
    return C; // Return result matrix A x B = C
}
