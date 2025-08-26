//
// Created by luca eaton on 8/18/25.
//

#ifndef MATRIX_H
#define MATRIX_H
#include <vector>


class Matrix {
    public:

        Matrix();
        Matrix(int rows, int cols);

        static Matrix Multiply(const Matrix& m1, const Matrix& m2);
        static Matrix addMatrix(const Matrix &m1, const Matrix &m2);
        static Matrix scaleMatrix(const int &x, const Matrix &m);
        static Matrix transposeMatrix(const Matrix &m);
        static Matrix broadcastMatrix(const Matrix &m1, const Matrix &m2);

        double dot(const Matrix &m1) const;

        //Getters and Setters
        void setMatrixData(std::vector<std::vector<double>> m);
        const std::vector<std::vector<double>> &getMatrix();

    private:
        int rows;
        int cols;
        std::vector<std::vector<double>> data;

};



#endif //MATRIXES_H
