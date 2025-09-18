//
// Created by luca eaton on 8/18/25.
//

#ifndef MATRIX_H
#define MATRIX_H
#include <span>
#include <vector>


class Matrix {
    public:

        Matrix();
        Matrix(int rows, int cols);

        static Matrix Multiply(const Matrix& m1, const Matrix& m2);
        static Matrix addMatrix(const Matrix &m1, const Matrix &m2);

        static Matrix subtractMatrix(const Matrix &m1, const Matrix &m2);

        static Matrix scaleMatrix(const int &x, const Matrix &m);
        static Matrix transposeMatrix(const Matrix &m);
        static Matrix broadcastMatrix(const Matrix &m1, const Matrix &m2);

        [[nodiscard]] double dot(const Matrix &m1) const;

        [[nodiscard]] double dotRowVector(const Matrix &m1) const;

        //Getters and Setters
        void setMatrixData(std::vector<std::vector<double>> m);

        void setValue(int r, int c, double value);

        const std::vector<std::vector<double>> &getMatrix();

        [[nodiscard]] int getRow() const;

        [[nodiscard]] int getCol() const;

        [[nodiscard]] Matrix getRowVector(int r) const;

        std::span<double> rowSpan(int r);

        [[nodiscard]] std::span<const double> rowSpan(int r) const;;


    private:
        int rows;
        int cols;
        std::vector<std::vector<double>> data;

};

#endif //MATRIX_H
