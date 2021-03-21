#ifndef MATRIX_H
#define MATRIX_H

#include <iostream>

namespace MatrixArchetype{
    static const char ZEROS = '0';
    static const char ONES = '1';
    static const char ID = 'I';
}

class Matrix{
    private:
        int rows;
        int columns;
        int elements;
        int type;
        double* data;

    public:
        explicit Matrix(int rows, int columns, double* data);
        Matrix(int rows, int columns, char archetype);
        Matrix(const Matrix& m);
        ~Matrix();

        void putColumn(int j, double* elems);
        void putRow(int i, double* elems);
        void put(int i, int j, double elem);
        void putSubmatrix(int startRow, int startColumn, Matrix const& m);

        double get(int i, int j);
        double* getRow(int i);
        double* getColumn(int j);

        double* getData();
        int getRows();
        int getColumns();
        int getType();
        int getElements(); 

        void setData(double* data);

        Matrix operator*(Matrix const& other);
        Matrix operator+(Matrix const& other);
        Matrix operator-(Matrix const& other);
        Matrix& operator*=(Matrix const& other);
        Matrix& operator+=(Matrix const& other);
        Matrix& operator-=(Matrix const& other);
        bool operator==(Matrix const& other);
        bool operator!=(Matrix const& other);

        friend std::ostream& operator<<(std::ostream& os, const Matrix& m);
};


#endif