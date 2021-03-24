#ifndef MATRIX_H
#define MATRIX_H

#include <iostream>
#include <type_traits>

namespace MatrixArchetype{
    static const char ZEROS = '0';
    static const char ONES = '1';
    static const char ID = 'I';
}

template<typename T>
class Matrix{

    static_assert(std::is_arithmetic<T>::value, "Matrix cannot be instantiated with a non-numeric type");

    private:
        int rows;
        int columns;
        int elements;
        int type;
        T* data;

    public:
        explicit Matrix(int rows, int columns, T* data);
        Matrix(int rows, int columns, char archetype=MatrixArchetype::ZEROS);
        Matrix(const Matrix& m);
        ~Matrix();

        void putColumn(int j, T* elems);
        void putRow(int i, T* elems);
        void put(int i, int j, T elem);
        void putSubmatrix(int startRow, int startColumn, Matrix const& m);

        T get(int i, int j);
        T* getRow(int i);
        T* getColumn(int j);

        T* getData();
        int getRows();
        int getColumns();
        int getType();
        int getElements(); 

        void setData(T* data);

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