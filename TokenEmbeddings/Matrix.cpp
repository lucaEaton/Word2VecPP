//
// Created by luca eaton on 8/18/25.
//

#include "Matrix.h"
#include <iostream>

#include "FileReader.h"
#include <span>
using namespace std;
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
Matrix::Matrix(int rows, int cols)
    : rows(rows), cols(cols),
      data(rows, std::vector<double>(cols, 0)) {}
/**
 *
 * @param m1 Matrix A
 * @param m2 Matrix B
 *
 * NOTE TO SELF : Essential for layer to layer transformations
 *
 * @return The multiplied sum of the two given matrix's
 */
Matrix Matrix::Multiply(const Matrix& m1, const Matrix& m2) {
    Matrix m3(m1.rows,m2.cols);
    for (int i = 0; i < m1.rows; i++) {
        for (int j = 0; j < m2.cols; j++) {
            m3.data[i][j] = 0;
            for (int k = 0; k < m1.cols; k++) {
                m3.data[i][j] += m1.data[i][k] * m2.data[k][j];
            }
        }
    }
    return m3;
}
/**
 *
 * @param m1 Matrix A
 * @param m2 Matrix B
 *
 * NOTE TO SELF : For applying activations or adjusting weights
 *
 * @return The sum of the two given matrix's
 */
Matrix Matrix::addMatrix(const Matrix& m1, const Matrix& m2) {
    if (m1.rows != m2.rows || m1.cols != m2.cols)
        throw std::invalid_argument("invalid matrix size");

    Matrix m3(m1.rows, m2.cols);
    for (int i = 0; i < m1.rows; i++) {
        for (int j = 0; j < m2.cols; j++) {
            m3.data[i][j] += m1.data[i][j] + m2.data[i][j];
        }
    }
    return m3;
}
/**
 *
 * @param m1 Matrix A
 * @param m2 Matrix B
 *
 * NOTE TO SELF : For applying activations or adjusting weights
 *
 * @return The sum of the two given matrix's
 */
Matrix Matrix::subtractMatrix(const Matrix& m1, const Matrix& m2) {
    if (m1.rows != m2.rows || m1.cols != m2.cols)
        throw std::invalid_argument("invalid matrix size");

    Matrix m3(m1.rows, m2.cols);
    for (int i = 0; i < m1.rows; i++) {
        for (int j = 0; j < m2.cols; j++) {
            m3.data[i][j] -= m1.data[i][j] + m2.data[i][j];
        }
    }
    return m3;
}
/**
 *
 * @param x the scalar
 * @param m Matrix A
 *
 * NOTE TO SELF : For applying activations or adjusting weights
 *
 * @return The matrix multiplied by the scalar
 */
Matrix Matrix::scaleMatrix(const int& x,const Matrix& m ) {
    Matrix m3(m.rows, m.cols);
    for (int i = 0; i < m.rows; i++) {
        for (int j = 0; j < m.cols; j++) {
            m3.data[i][j] = m.data[i][j] * x;
        }
    }
    return m3;
}
/**
 *
 * @param m Matrix A
 * @return Matric A transposed
 */
Matrix Matrix::transposeMatrix(const Matrix& m) {
    Matrix m3(m.cols,m.rows);
    for (int i = 0; i < m.rows; ++i) {
        for (int j = 0; j < m.cols; ++j) {
            m3.data[j][i] = m.data[i][j];
        }
    }
    return m3;
}
/**
 *
 * @param m1 Matrix A
 * @param m2 Matrix B
 *
 * NOTE TO SELF : To add bias vectors across rows
 *
 * @return Broadcasted sum of matrix a + b
 */
Matrix Matrix::broadcastMatrix(const Matrix& m1, const Matrix& m2) {
    bool rowCheck = (m1.rows == m2.rows) || (m1.rows == 1) || (m2.rows == 1);
    bool colCheck = (m1.cols == m2.cols) || (m1.cols == 1) || (m2.cols == 1);
    if (!rowCheck || !colCheck)
        throw std::invalid_argument("broadcastMatrix: incompatible shapes for broadcasting");

    int newRowSize = std::max(m1.rows, m2.rows);
    int newColSize = std::max(m1.cols, m2.cols);
    Matrix m3(newRowSize, newColSize);
    for (int i = 0; i < newRowSize; ++i) {
        int m1_i = (m1.rows == 1) ? 0 : i;
        int m2_i = (m2.rows == 1) ? 0 : i;
        for (int j = 0; j < newColSize; ++j) {
            int m1_j = (m1.cols == 1) ? 0 : j;
            int m2_j = (m2.cols == 1) ? 0 : j;
            m3.data[i][j] = m1.data[m1_i][m1_j] + m2.data[m2_i][m2_j];
        }
    }
    return m3;
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
            result += this->data[i][j] * m1.data[i][j];
        }
    }
    return result;
}
/*
 * Dot Product for single rows
 * @param m1 Matrix u wish to apply dot product with
 * @return Dot product of both matrices
 *
 */
double Matrix::dotRowVector(const Matrix& m1) const {
    if (rows != 1 || m1.rows != 1 || cols != m1.cols) {
        throw std::invalid_argument("Both must be row vectors of the same length.");
    }

    double result = 0.0;
    for (int j = 0; j < cols; j++) {
        result += data[0][j] * m1.data[0][j];
    }
    return result;
}
/**
 *
 * @param m Matrix u wish to set.
 */
void Matrix::setMatrixData(vector<vector<double>> m) {
    data = m;
    rows = m.size();
    cols = m[0].size();
}
/**
 *
 * @param Set of rows and col to be a certain value
 */
void Matrix::setValue(const int r, const int c, const double value) {
    data[r][c] = value;
}
/**
 *
 * @return The Matrix's Data
 */
const vector<vector<double>>& Matrix::getMatrix() {
    return data;
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
 * @param r Row we wish to retrieve
 * @return the row and its values
 */
Matrix Matrix::getRowVector(int r) const {
    if (r < 0 || r >= rows) {
        throw std::out_of_range("Row index out of range");
    }

    Matrix rowMatrix(1, cols);
    for (int j = 0; j < cols; ++j) {
        rowMatrix.setValue(0, j, data[r][j]);  // assuming data[row][col] layout
    }

    return rowMatrix;
}
/**
 *
 * @param r Row index
 * @return Row without making a copy
 */
std::span<double> Matrix::rowSpan(int r) {
    if (r < 0 || r >= rows) {
        throw std::out_of_range("row out of range");
    }
    return std::span<double>(data[r].data(), cols);
}

std::span<const double> Matrix::rowSpan(int r) const {
    if (r < 0 || r >= rows) {
        throw std::out_of_range("row out of range");
    }
    return std::span<const double>(data[r].data(), cols);
}

ostream operator<<(const ostream & lhs, const Matrix & rhs);

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



