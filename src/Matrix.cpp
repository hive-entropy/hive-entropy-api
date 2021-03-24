#include "Matrix.h"

#include <malloc.h>
#include <cblas.h>

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