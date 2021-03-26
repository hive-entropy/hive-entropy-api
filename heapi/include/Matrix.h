#ifndef MATRIX_H
#define MATRIX_H

#include <iostream>
#include <type_traits>
#include <malloc.h>
#include <cblas.h>

void something();

namespace MatrixArchetype{
    static const char ZEROS = '0';
    static const char ONES = '1';
    static const char ID = 'I';
}


template<typename T>
class Matrix{
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

template<typename T>
Matrix<T>::~Matrix(){
    //free(data);
}

template<typename T>
Matrix<T>::Matrix(int rows, int columns, T* data) : rows(rows), columns(columns){
    this->data = (T*) malloc(rows*columns*sizeof(T));
    for(int i=0;i<rows;i++)
        for(int j=0;j<columns;j++)
            this->data[columns*i+j] = data[columns*i+j];
}

template<typename T>
Matrix<T>::Matrix(int rows, int columns, char archetype) : rows(rows), columns(columns){
    elements = rows*columns;
    data = (T*) malloc(rows*columns*sizeof(T));
    switch(archetype){
        case MatrixArchetype::ZEROS:
            for(int i=0;i<rows;i++)
                for(int j=0;j<columns;j++)
                    data[columns*i+j] = (T) 0.0;
        case MatrixArchetype::ONES:
            for(int i=0;i<rows;i++)
                for(int j=0;j<columns;j++)
                    data[columns*i+j] = (T) 0.0;
        case MatrixArchetype::ID:
            for(int i=0;i<rows;i++)
                for(int j=0;j<columns;j++)
                    data[columns*i+j] = (i==j)? (T) 1.0 : (T) 0.0;
    }
}

template<typename T>
Matrix<T>::Matrix(const Matrix& m){
    rows = m.rows;
    columns = m.columns;
    elements = rows*columns;
    this->data = (double*) malloc(rows*columns*sizeof(double));
    for (int i=0;i<rows;i++)
        for(int j=0;j<columns;j++)
            data[columns*i+j] = m.data[m.columns*i+j];
}

#endif