//
// file: Matrix.h
// desc: ACS Project 2 Matrix Class Header
// auth: Andrew Prata
//
// This program performs matrix-matrix multiplication.
// It includes three optimization options for this
// operation. These are: multithreading, SIMD, and
// cache usage optimization.
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