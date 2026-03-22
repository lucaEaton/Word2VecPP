//
// Created by luca eaton on 8/18/25.
//

#include "Matrix.h"
#include <iostream>
#include "../Files/FileReader.h"
#include <span>
/**
 * Blank Constructor Method
 */
Matrix::Matrix() {
    rows = 0;
    cols = 0;
}
/**
 * Constructor Method
 *
 * @param rows the # of rows
 * @param cols the # of cols
 */
Matrix::Matrix(const int rows, const int cols)
    : rows(rows), cols(cols),
      data(rows * cols, 0.0) {}

void Matrix::addValue(const double val) {
    data.push_back(val);
}
size_t Matrix::dataSize() const {
    return data.size();
}
/**
 * Dot Product
 * @param m1 Matrix u wish to apply dot product with
 * @return Dot product of both matrices
 */
double Matrix::dot(const Matrix& m1) const {
    double result = 0.0;

    // Make sure dimensions match!
    if (rows != m1.rows || cols != m1.cols) {
        throw std::invalid_argument("Matrix dimensions must match for dot product.");
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            result += this->data[i * cols + j] * m1.data[i * m1.cols + j];
        }
    }
    return result;
}

void Matrix::setValue(const int r, const int c, const double value) {
    data[r * c + c] = value;
}
/**
 *
 * @return # of rows
 */
int Matrix::getRow() const {
    return rows;
}
/**
 *
 * @return # of cols
 */
int Matrix::getCol() const {
    return cols;
}
/**
 *
 * @param r Row index
 * allows to write to
 * @return Row without making a copy
 */
std::span<double> Matrix::rowSpan(const int r) {
    if (r < 0 || r >= rows) {
        throw std::out_of_range("row out of range");
    }
    return std::span(data.data() + r * cols, cols);
}
// only read
std::span<const double> Matrix::rowSpan(const int r) const {
    if (r < 0 || r >= rows) {
        throw std::out_of_range("row out of range");
    }
    return std::span(data.data() + r * cols, cols);
}

/*
int main() {
    Matrix A(2, 1);
    Matrix B(2, 2);

    Matrix Z = FileReader::loadEmbeddingsToMatrix("./Files/VocabEmbeddings.txt",(71294 - 5), 100);

    const std::vector<std::vector<double>> vals = {
        {1, 5},
        {4, 5}
    };
    A.setMatrixData(vals);

    const std::vector<std::vector<double>> vals2 = {
        {1, 2},
        {4, 5}
    };
    B.setMatrixData(vals2);

    const auto C = A.dot(B);


    std::cout << "Result:\n";
    for (auto &row : Z) {
        std::cout << "{ ";
        for (auto &val : row) {
            std::cout << val << " ";
        }
        std::cout << "}" << "\n";
    }


    std::cout << "Z = " << Z << std::endl;
    return 0;
}
*/



