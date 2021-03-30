#ifndef MATRIX_H
#define MATRIX_H

#define ARRAY_SIZE(x)  (sizeof(x) / sizeof((x)[0]))

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

    static_assert(std::is_arithmetic<T>::value, "The Matrix type must be an arithmetic type");

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
        Matrix getSubmatrix(int startRow, int startColumn, int endRow, int endColumn);

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

template<typename T>
void Matrix<T>::putColumn(int j, T* elems){
    if(j<0||j>=columns||ARRAY_SIZE(elems)!=rows)
        throw "Whatever";
    for(int i=0;i<rows;i++)
        data[columns*i+j] = elems[i];
}

template<typename T>
void Matrix<T>::putRow(int i, T* elems){
    if(i<0||i>=rows||ARRAY_SIZE(elems)!=columns)
        throw "Whatever";
    for(int j=0;j<columns;j++)
        data[columns*i+j] = elems[j];
}


template<typename T>
void Matrix<T>::put(int i, int j, T elem){
    if(i<0||i>=rows||j<0||j>=columns)
        throw "Whatever";
    data[columns*i+j] = elem;
}

template<typename T>
void Matrix<T>::putSubmatrix(int startRow, int startColumn, Matrix const& m){
    if(startColumn+m.columns>=columns||startRow+m.rows>=rows)
        throw "Whatever";
    for(int i=startRow;i<m.rows+startRow;i++){
        for(int j=startColumn;j<m.columns+startColumn;j++){
            data[columns*i+j] = static_cast<T>(m.data[m.columns*i+j]);
        }
    }
}

template<typename T>
Matrix<T> Matrix<T>::getSubmatrix(int startRow, int startColumn,int endRow, int endColumn){
    if(startColumn<0||startColumn>columns||endColumn<startColumn||endColumn<columns||startRow<0||startRow>rows||startRow>endRow||endRow>rows||endRow<0)
        throw "Whatever";
    Matrix<T> res(endRow-startRow,endColumn-startColumn);
    for(int i=startRow;i<=endRow;i++){
        for(int j=startColumn;j<=endColumn;j++){
            res.put(i,j,data[columns*i+j]);
        }
    }
    return res;
}

template<typename T>
T Matrix<T>::get(int i, int j){
    if(i<0||i>=rows||j<0||j>=columns)
        throw "Whatever";
    return data[columns*i+j];
}

template<typename T>
T* Matrix<T>::getRow(int i){
    if(i<0||i>=rows)
        throw "Whatever";
    T* res = (T*) malloc(columns*sizeof(T));
    for(int j=0;j<columns;j++)
        res[j] = data[columns*i+j];
    return res;
}

template<typename T>
T* Matrix<T>::getColumn(int j){
    if(j<0||j>=columns)
        throw "Whatever";
    T* res = (T*) malloc(rows*sizeof(T));
    for(int i=0;i<rows;i++)
        res[i] = data[columns*i+j];
    return res;
}

template<typename T>
T* Matrix<T>::getData(){
    return data;
}

template<typename T>
int Matrix<T>::getRows(){
    return rows;
}

template<typename T>
int Matrix<T>::getColumns(){
    return columns;
}

template<typename T>
int Matrix<T>::getType(){
    return type;
}

template<typename T>
int Matrix<T>::getElements(){
    return elements;
}

#endif