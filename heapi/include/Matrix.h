#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-exception-baseclass"
#ifndef MATRIX_H
#define MATRIX_H

#include <iostream>
#include <type_traits>
#include <malloc.h>
#include <openblas/cblas.h>
#include <functional>
#include <cmath>
#include <map>
#include <algorithm>

#include "IncompatibleDimensionsException.h"
#include "OutOfBoundException.h"

namespace MatrixArchetype{
    static const char ZEROS = '0';
    static const char ONES = '1';
    static const char ID = 'I';
}

enum EdgeHandling {
    Extend,
    Wrap,
    Mirror,
    Crop,
    KernelCrop
};

enum ImagePostProcess {
    Normalize,
    Clamp
};

template<typename T>
class Matrix{

    static_assert(std::is_arithmetic<T>::value, "The Matrix type must be an arithmetic type");

    private:
        int rows;
        int columns;
        int elements;
        int type;
        std::vector<T> data;

        template<typename M>
        static std::map<EdgeHandling, std::function< Matrix<T> (Matrix<T> const &matrix, Matrix<M> const &mask, ImagePostProcess const &postProcess) >> edgeHandlingMethods;
        template<typename M>
        static Matrix<T> extendConvolve(Matrix<T> const &matrix, Matrix<M> const &mask, ImagePostProcess const &postProcess);
        template<typename M>
        static Matrix<T> wrapConvolve(Matrix<T> const &matrix, Matrix<M> const &mask, ImagePostProcess const &postProcess);
        template<typename M>
        static Matrix<T> mirrorConvolve(Matrix<T> const &matrix, Matrix<M> const &mask, ImagePostProcess const &postProcess);
        template<typename M>
        static Matrix<T> cropConvolve(Matrix<T> const &matrix, Matrix<M> const &mask, ImagePostProcess const &postProcess);
        template<typename M>
        static Matrix<T> kernelCropConvolve(Matrix<T> const &matrix, Matrix<M> const &mask, ImagePostProcess const &postProcess);

    public:
        // Constructors
        template<typename M>
        Matrix(int const &rows, int const &columns, M const *data);
        Matrix(int const &rows, int const &columns, char const &archetype= MatrixArchetype::ZEROS);
        template<typename M>
        explicit Matrix(Matrix<M> const &m);

        // Getters
        [[nodiscard]] int getRows() const;
        [[nodiscard]] int getColumns() const;
        [[nodiscard]] int getType() const;
        [[nodiscard]] int getElements() const;

        [[nodiscard]] T get(int const &i, int const &j) const;
        [[nodiscard]] T* getRow(int const &i) const;
        [[nodiscard]] T* getColumn(int const &j) const;

        [[nodiscard]] Matrix getSubmatrix(int const &startRow, int const &startColumn, int const &endRow, int const &endColumn) const;

        T* getData();
        [[nodiscard]] T const* getData() const;

        T* operator[](int const &index);
        T const* operator[](int const &index) const;

        // Setters
        void put(int const &i, int const &j, T elem);
        void putRow(int const &i, T const *elems);
        void putColumn(int const &j, T const *elems);
        void putSubmatrix(int const &startRow, int const &startColumn, Matrix const &m);
        void setData(const T *data);

        // Operations
        template<typename M>
        Matrix convolve(Matrix<M> const &mask, EdgeHandling const &edgeHandler = EdgeHandling::Extend, ushort const &nbIteration = 1, ImagePostProcess const &postProcess = ImagePostProcess::Normalize) const;
        Matrix operator*(Matrix<T> const &other);
        Matrix operator+(Matrix<T> const &other);
        Matrix operator-(Matrix<T> const &other);
        Matrix& operator*=(Matrix<T> const &other);
        Matrix& operator+=(Matrix<T> const &other);
        Matrix& operator-=(Matrix<T> const &other);
        bool operator==(Matrix<T> const &other) const;
        bool operator!=(Matrix<T> const &other) const;

        void show() const;
        [[nodiscard]] std::string toString() const;
};

template<typename T>
template<typename M>
Matrix<T>::Matrix(int const &rows, int const &columns, M const *data) : rows(rows), columns(columns) {
    static_assert(std::is_arithmetic<M>::value, "The Matrix type must be an arithmetic type");
    elements = rows*columns;
    // Copy the input data to the intern data vector
    this->data.resize(elements);
    for(int i=0;i<rows;i++)
        for(int j=0;j<columns;j++)
            this->data[columns*i+j] = static_cast<T>(data[columns*i+j]);
}

template<typename T>
Matrix<T>::Matrix(int const &rows, int const &columns, char const &archetype) : rows(rows), columns(columns){
    elements = rows*columns;
    this->data.resize(elements);
    switch(archetype){
        case MatrixArchetype::ZEROS:
            for(int i=0;i<rows;i++)
                for(int j=0;j<columns;j++)
                    data[columns*i+j] = static_cast<T>(0.0);
        break;
        case MatrixArchetype::ONES:
            for(int i=0;i<rows;i++)
                for(int j=0;j<columns;j++)
                    data[columns*i+j] = static_cast<T>(1.0);
        break;
        case MatrixArchetype::ID:
            for(int i=0;i<rows;i++)
                for(int j=0;j<columns;j++)
                    data[columns*i+j] = (i==j)? static_cast<T>(1.0) : static_cast<T>(0.0);
        break;
    }
}

template<typename T>
template<typename M>
Matrix<T>::Matrix(Matrix<M> const &m){
    static_assert(std::is_arithmetic<M>::value, "The Matrix type must be an arithmetic type");
    rows = m.getRows();
    columns = m.getColumns();
    elements = rows*columns;
    this->data.resize(elements);
    for (int i=0;i<rows;i++)
        for(int j=0;j<columns;j++)
            data[columns*i+j] = static_cast<T>(m.getData()[m.getColumns() * i + j]);
}

template<typename T>
void Matrix<T>::putColumn(int const &j, T const *elems){
    if(j<0||j>=columns)
        throw OutOfBoundException("j",j,std::vector<int>(0,columns));
    for(int i=0;i<rows;i++)
        data[columns*i+j] = elems[i];
}

template<typename T>
void Matrix<T>::putRow(int const &i, T const *elems){
    if(i<0||i>=rows)
        throw OutOfBoundException("i",i,std::vector<int>(0,rows));
    for(int j=0;j<columns;j++)
        data[columns*i+j] = elems[j];
}

template<typename T>
void Matrix<T>::put(int const &i, int const &j, T elem){
    if(i<0||i>=rows)
        throw OutOfBoundException("i",i,std::vector<int>(0,rows));
    else if(j<0||j>=columns)
        throw OutOfBoundException("j",j,std::vector<int>(0,columns));
    data[columns*i+j] = elem;
}

template<typename T>
void Matrix<T>::putSubmatrix(int const &startRow, int const &startColumn, Matrix const &m){
    if(startColumn+m.columns-1>=columns)
        throw OutOfBoundException("m.columns",m.columns,std::vector<int>(0,columns+1-startColumn));
    else if(startRow+m.rows-1>=rows)
        throw OutOfBoundException("m.rows",m.rows,std::vector<int>(0,rows-startRow+1));
    for(int i=startRow;i<m.rows+startRow;i++){
        for(int j=startColumn;j<m.columns+startColumn;j++){
            data[columns*i+j] = static_cast<T>(m.data[m.columns*(i-startRow)+(j-startColumn)]);
        }
    }
}

template<typename T>
Matrix<T> Matrix<T>::getSubmatrix(int const &startRow, int const &startColumn,int const &endRow, int const &endColumn) const {
    if(startColumn<0||startColumn>=columns||endColumn<startColumn||endColumn>=columns||startRow<0||startRow>=rows||startRow>endRow||endRow>=rows||endRow<0)
        throw "Whatever";
    Matrix<T> res(endRow-startRow+1,endColumn-startColumn+1);
    for(int i=startRow;i<=endRow;i++){
        for(int j=startColumn;j<=endColumn;j++){
            res.put(i-startRow,j-startColumn,data[columns*i+j]);
        }
    }
    return res;
}

template<typename T>
T Matrix<T>::get(int const &i, int const &j) const {
    if(i<0||i>=rows)
        throw OutOfBoundException("i",i,std::vector<int>(0,rows));
    else if(j<0||j>=columns)
        throw OutOfBoundException("j",j,std::vector<int>(0,columns));
    return data[columns*i+j];
}

template<typename T>
T* Matrix<T>::getRow(int const &i) const {
    if(i<0||i>=rows)
        throw OutOfBoundException("i",i,std::vector<int>(0,rows));
    T* res = (T*) malloc(columns*sizeof(T));
    for(int j=0;j<columns;j++)
        res[j] = data[columns*i+j];
    return res;
}

template<typename T>
T* Matrix<T>::getColumn(int const &j) const {
    if(j<0||j>=columns)
        throw OutOfBoundException("j",j,std::vector<int>(0,columns));
    T* res = (T*) malloc(rows*sizeof(T));
    for(int i=0;i<rows;i++)
        res[i] = data[columns*i+j];
    return res;
}

template<typename T>
T* Matrix<T>::getData() {
    return data.data();
}
template<typename T>
T const* Matrix<T>::getData() const {
    return data.data();
}

template<typename T>
int Matrix<T>::getRows() const {
    return rows;
}

template<typename T>
int Matrix<T>::getColumns() const {
    return columns;
}

template<typename T>
int Matrix<T>::getType() const {
    return type;
}

template<typename T>
int Matrix<T>::getElements() const {
    return elements;
}

template<typename T>
Matrix<T>& Matrix<T>::operator*=(Matrix<T> const &other){
    if(columns!=other.rows)
        throw IncompatibleDimensionsException(rows, columns,other.rows,other.columns,Operation::MULTIPLICATION);
    Matrix<T> c(this->rows,other.columns,MatrixArchetype::ZEROS);
    T* c_tab = (T*) malloc(c.getRows()*c.getColumns()*sizeof(T));
    if(std::is_same<T,int>::value){
        float* trailedData = (float*) malloc(elements*sizeof(float));
        float* trailedOther = (float*) malloc(other.elements*sizeof(float));
        float* trailedC_tab = (float*) malloc(c.getElements()*sizeof(float));
        for(int i=0;i<elements;i++)
            trailedData[i] = static_cast<float>(data[i]);
        for(int i=0;i<other.elements;i++)
            trailedOther[i] = static_cast<float>(other.data[i]);

        cblas_sgemm(CblasRowMajor,CblasNoTrans,CblasNoTrans,this->rows,other.columns,this->columns,1.0,trailedData,this->columns,trailedOther,other.columns,0.0,trailedC_tab,other.columns);

        for(int i=0;i<c.getElements();i++)
            c_tab[i] = static_cast<T>(trailedC_tab[i]);
    }
    else if(std::is_same<T,float>::value){
        cblas_sgemm(CblasRowMajor,CblasNoTrans,CblasNoTrans,this->rows,other.columns,this->columns,1.0,reinterpret_cast<const float*>(data.data()),this->columns,reinterpret_cast<const float*>(other.data.data()),other.columns,0.0,reinterpret_cast<float*>(c_tab),other.columns);
    }
    else if(std::is_same<T,double>::value){
        cblas_dgemm(CblasRowMajor,CblasNoTrans,CblasNoTrans,this->rows,other.columns,this->columns,1.0,reinterpret_cast<const double*>(data.data()),this->columns,reinterpret_cast<const double*>(other.data.data()),other.columns,0.0,reinterpret_cast<double*>(c_tab),other.columns);
    }
    else{
        throw "Matrix multiplication is not supported for this type";
    }
    c.setData(c_tab);
    *this = c;
    return *this;
}

template<typename T>
void Matrix<T>::setData(const T *data){
    for(int i=0;i<rows;i++)
        for(int j=0;j<columns;j++)
            this->data[columns*i+j] = data[columns*i+j];
}

template<typename T>
Matrix<T>& Matrix<T>::operator+=(Matrix<T> const &other){
    if(other.columns!=columns||other.rows!=rows)
        throw IncompatibleDimensionsException(rows, columns,other.rows,other.columns,Operation::ADDITION);
    for (int i=0;i<rows;i++)
        for(int j=0;j<columns;j++)
            data[columns*i+j] += static_cast<T>(other.data[other.columns*i+j]);
    return *this;
}

template<typename T>
Matrix<T>& Matrix<T>::operator-=(Matrix<T> const &other){
    if(other.columns!=columns||other.rows!=rows)
        throw IncompatibleDimensionsException(rows, columns,other.rows,other.columns,Operation::SUBTRACTION);
    for (int i=0;i<rows;i++)
        for(int j=0;j<columns;j++)
            data[columns*i+j] -= static_cast<T>(other.data[other.columns*i+j]);
    return *this;
}

template<typename T>
Matrix<T> Matrix<T>::operator*(Matrix<T> const &other){
    if(columns!=other.rows)
        throw IncompatibleDimensionsException(rows, columns,other.rows,other.columns,Operation::MULTIPLICATION);
    Matrix res = *this;
    return res *= other;
}

template<typename T>
Matrix<T> Matrix<T>::operator+(Matrix<T> const &other){
    if(other.columns!=columns||other.rows!=rows)
        throw IncompatibleDimensionsException(rows, columns,other.rows,other.columns,Operation::ADDITION);
    Matrix res = *this;
    return res += other;
}

//Check for equal sizes
template<typename T>
Matrix<T> Matrix<T>::operator-(Matrix<T> const &other){
    if(other.columns!=columns||other.rows!=rows)
        throw IncompatibleDimensionsException(rows, columns,other.rows,other.columns,Operation::SUBTRACTION);
    Matrix res = *this;
    return res -= other;
}

template<typename T>
bool Matrix<T>::operator==(Matrix<T> const &other) const{
    if(columns!=other.columns||rows!=other.rows) return false;
    for(int i=0;i<rows;i++)
        for(int j=0;j<columns;j++)
            if(data[columns*i+j]!=other.data[other.columns*i+j]) return false;
    return true;
}

template<typename T>
bool Matrix<T>::operator!=(Matrix<T> const &other) const{
    return !(*this==other);
}

template<typename T>
T* Matrix<T>::operator[](int const &index) {
    return &data.data()[index * columns];
}
template<typename T>
T const* Matrix<T>::operator[](int const &index) const {
    return &data.data()[index * columns];
}

template<typename T>
void Matrix<T>::show() const {
    std::cout << toString() << std::endl;
}

template<typename T>
std::string Matrix<T>::toString() const {
    std::string output;

    for(int i = 0 ; i < rows ; i++)
    {
        output += "[ ";
        for(int j = 0 ; j < columns ; j++)
        {
            output += std::to_string(data[columns * i + j]);
            output += " ";
        }
        output += "]\n";
    }

    return output;
}

template<typename T>
std::ostream& operator<< (std::ostream& os, Matrix<T> value) {
    os << value.toString();
    return os;
}

/*////////////////////////////////
///     MATRIX CONVOLUTION     ///
////////////////////////////////*/

template<typename T>
template<typename M>
std::map<EdgeHandling, std::function< Matrix<T> (Matrix<T> const &matrix, Matrix<M> const &mask, ImagePostProcess const &postProcess) >> Matrix<T>::edgeHandlingMethods = {
        {EdgeHandling::Extend, Matrix<T>::extendConvolve<M>},
        {EdgeHandling::Wrap, Matrix<T>::wrapConvolve<M>},
        {EdgeHandling::Mirror, Matrix<T>::mirrorConvolve<M>},
        {EdgeHandling::Crop, Matrix<T>::cropConvolve<M>},
        {EdgeHandling::KernelCrop, Matrix<T>::kernelCropConvolve<M>}
};

template<typename T>
template<typename M>
Matrix<T> Matrix<T>::extendConvolve(Matrix<T> const &matrix, Matrix<M> const &mask, ImagePostProcess const &postProcess) {
    static_assert(std::is_arithmetic<M>::value, "The kernel type must be an arithmetic type");
    // Compute the mask offset
    int offset = std::floor((double) mask.getRows() / 2.0);
    // Create the result matrix
    Matrix<T> result(matrix.rows + offset, matrix.columns + offset, MatrixArchetype::ZEROS);
    // Place old data into the result matrix
    for (int row = 0; row < matrix.rows; ++row) {
        for (int col = 0; col < matrix.columns; ++col) {
            result[row + offset][col + offset] = matrix[row][col];
        }
    }
    // Extend the matrix data
    while (offset > 0) {
        /*// Fill the borders
        for (int row = offset; row < result.rows - offset; ++row) {
            result[row][offset - 1] = result[row][offset];
            result[row][result.columns - offset + 1] = result[row][result.columns - offset];
        }
        for (int col = offset; col < result.columns - offset; ++col) {
            result[offset - 1][col] = result[offset][col];
            result[result.rows - offset + 1][col] = result[result.rows - offset][col];
        }*/
        // Fill the corners
        /*result[offset - 1][offset - 1] = result[offset][offset];
        result[result.rows - offset + 1][offset - 1] = result[result.rows - offset][offset];
        result[offset + 1][result.columns - offset + 1] = result[offset][result.columns - offset];
        result[result.rows - offset + 1][result.columns - offset + 1] = result[result.rows - offset][result.columns - offset];*/

        offset--;
    }
    // Convolve the matrix and return the result
    return cropConvolve(result, mask, postProcess);
}

template<typename T>
template<typename M>
Matrix<T> Matrix<T>::wrapConvolve(Matrix<T> const &matrix, Matrix<M> const &mask, ImagePostProcess const &postProcess) {
    throw "Not implemented yet!";
}

template<typename T>
template<typename M>
Matrix<T> Matrix<T>::mirrorConvolve(Matrix<T> const &matrix, Matrix<M> const &mask, ImagePostProcess const &postProcess) {
    throw "Not implemented yet!";
}

template<typename T>
template<typename M>
Matrix<T> Matrix<T>::cropConvolve(Matrix<T> const &matrix, Matrix<M> const &mask, ImagePostProcess const &postProcess) {
    // Compute the mask maskSum
    float maskSum = 0;
    for (int i = 0; i < mask.getElements(); i++) {
        maskSum += mask.getData()[i];
    }
    // Compute the mask offset
    int offset = std::floor(mask.getRows() / 2.0);
    // Create the interMatrix matrix
    Matrix<int> interMatrix(matrix.rows - offset * 2, matrix.columns - offset * 2, MatrixArchetype::ZEROS);
    // Create the accumulator
    float accumulator;
    int minValue = INT16_MAX, maxValue = INT16_MIN;

    // For each pixel in the interMatrix matrix
    for (int row = 0; row < interMatrix.getRows(); ++row) {
        for (int col = 0; col < interMatrix.getColumns(); ++col) {
            // Reset the accumulator
            accumulator = 0;

            // For each pixel in the mask
            for (int maskRow = 0; maskRow < mask.getRows(); ++maskRow) {
                for (int maskCol = 0; maskCol < mask.getColumns(); ++maskCol) {
                    T matrixValue = matrix[row + offset + maskRow][col + offset + maskCol];
                    M maskValue = mask[maskRow][maskCol];
                    accumulator += (matrixValue * maskValue);
                }
            }

            // Update the interMatrix matrix's pixel
            float newValue = maskSum != 0 ? accumulator / maskSum : accumulator;
            minValue = static_cast<int>(fmin(minValue, newValue));
            maxValue = static_cast<int>(fmax(maxValue, newValue));
            interMatrix[row][col] = static_cast<int>(newValue);
        }
    }

    // Clamp values to 0-255
    if (postProcess == ImagePostProcess::Clamp){
        for (int row = 0; row < interMatrix.getRows(); ++row)
        {
            for (int col = 0; col < interMatrix.getColumns(); ++col)
            {
                interMatrix[row][col] = std::clamp(interMatrix[row][col], 0, 255);
            }
        }
    }

    // Normalise values to 0-255
    if (postProcess == ImagePostProcess::Normalize) {
        for (int row = 0; row < interMatrix.getRows(); ++row)
        {
            for (int col = 0; col < interMatrix.getColumns(); ++col)
            {
                if (maxValue == 0) maxValue = 1;
                interMatrix[row][col] = (interMatrix[row][col] - minValue) * 255 / maxValue;
            }
        }
    }

    Matrix<T> result(interMatrix);

    return result;
}

template<typename T>
template<typename M>
Matrix<T> Matrix<T>::kernelCropConvolve(Matrix<T> const &matrix, Matrix<M> const &mask, ImagePostProcess const &postProcess) {
    throw "Not implemented yet!";
}

template<typename T>
template<typename M>
Matrix<T> Matrix<T>::convolve(Matrix<M> const &mask, EdgeHandling const &edgeHandler, ushort const &nbIteration, ImagePostProcess const &postProcess) const {
    Matrix convolution = edgeHandlingMethods<M>[edgeHandler](*this, mask, postProcess);
    for (int i = 1; i < nbIteration; ++i) {
        convolution = edgeHandlingMethods<M>[edgeHandler](convolution, mask, postProcess);
    }
    return convolution;
}

#endif
#pragma clang diagnostic pop