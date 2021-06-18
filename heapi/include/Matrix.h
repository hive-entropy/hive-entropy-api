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

void something();

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
        static std::map<EdgeHandling, std::function< Matrix<T> (Matrix<T> matrix, Matrix<M> mask, ImagePostProcess postProcess) >> edgeHandlingMethods;
        template<typename M>
        static Matrix<T> extendConvolve(Matrix<T> matrix, Matrix<M> mask, ImagePostProcess postProcess);
        template<typename M>
        static Matrix<T> wrapConvolve(Matrix<T> matrix, Matrix<M> mask, ImagePostProcess postProcess);
        template<typename M>
        static Matrix<T> mirrorConvolve(Matrix<T> matrix, Matrix<M> mask, ImagePostProcess postProcess);
        template<typename M>
        static Matrix<T> cropConvolve(Matrix<T> matrix, Matrix<M> mask, ImagePostProcess postProcess);
        template<typename M>
        static Matrix<T> kernelCropConvolve(Matrix<T> matrix, Matrix<M> mask, ImagePostProcess postProcess);

    public:
        template<typename M>
        Matrix(int rows, int columns, M* data);
        Matrix(int rows, int columns, char archetype=MatrixArchetype::ZEROS);
        template<typename M>
        explicit Matrix(Matrix<M>& m);
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
        int getRows() const;
        int getColumns() const;
        int getType() const;
        int getElements() const;

        void setData(T* data);

        template<typename M>
        Matrix convolve(Matrix<M> const &mask, EdgeHandling edgeHandler = EdgeHandling::Extend, ushort nbIteration = 1, ImagePostProcess postProcess = ImagePostProcess::Normalize);
        Matrix operator*(Matrix<T> const& other);
        Matrix operator+(Matrix<T> const& other);
        Matrix operator-(Matrix<T> const& other);
        Matrix& operator*=(Matrix<T> const& other);
        Matrix& operator+=(Matrix<T> const& other);
        Matrix& operator-=(Matrix<T> const& other);
        bool operator==(Matrix<T> const& other) const;
        bool operator!=(Matrix<T> const& other) const;
        T* operator[](int const& index);

        void show();
        std::string toString();
};

template<typename T>
Matrix<T>::~Matrix(){
}

template<typename T>
template<typename M>
Matrix<T>::Matrix(int rows, int columns, M* data) : rows(rows), columns(columns){
    static_assert(std::is_arithmetic<M>::value, "The Matrix type must be an arithmetic type");
    elements = rows*columns;
    // Copy the input data to the intern data vector
    this->data.resize(elements);
    for(int i=0;i<rows;i++)
        for(int j=0;j<columns;j++)
            this->data[columns*i+j] = static_cast<T>(data[columns*i+j]);
}

template<typename T>
Matrix<T>::Matrix(int rows, int columns, char archetype) : rows(rows), columns(columns){
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
Matrix<T>::Matrix(Matrix<M>& m){
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
void Matrix<T>::putColumn(int j, T* elems){
    if(j<0||j>=columns)
        throw new OutOfBoundException("j",j,std::vector<int>(0,columns));
    for(int i=0;i<rows;i++)
        data[columns*i+j] = elems[i];
}

template<typename T>
void Matrix<T>::putRow(int i, T* elems){
    if(i<0||i>=rows)
        throw new OutOfBoundException("i",i,std::vector<int>(0,rows));
    for(int j=0;j<columns;j++)
        data[columns*i+j] = elems[j];
}

template<typename T>
void Matrix<T>::put(int i, int j, T elem){
    if(i<0||i>=rows)
        new OutOfBoundException("i",i,std::vector<int>(0,rows));
    else if(j<0||j>=columns)
        throw new OutOfBoundException("j",j,std::vector<int>(0,columns));
    data[columns*i+j] = elem;
}

template<typename T>
void Matrix<T>::putSubmatrix(int startRow, int startColumn, Matrix const& m){
    if(startColumn+m.columns-1>=columns)
        throw new OutOfBoundException("m.columns",m.columns,std::vector<int>(0,columns+1-startColumn));
    else if(startRow+m.rows-1>=rows)
        throw new OutOfBoundException("m.rows",m.rows,std::vector<int>(0,rows-startRow+1));
    for(int i=startRow;i<m.rows+startRow;i++){
        for(int j=startColumn;j<m.columns+startColumn;j++){
            data[columns*i+j] = static_cast<T>(m.data[m.columns*(i-startRow)+(j-startColumn)]);
        }
    }
}

template<typename T>
Matrix<T> Matrix<T>::getSubmatrix(int startRow, int startColumn,int endRow, int endColumn){
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
T Matrix<T>::get(int i, int j){
    if(i<0||i>=rows)
        throw OutOfBoundException("i",i,std::vector<int>(0,rows));
    else if(j<0||j>=columns)
        throw OutOfBoundException("j",j,std::vector<int>(0,columns));
    return data[columns*i+j];
}

template<typename T>
T* Matrix<T>::getRow(int i){
    if(i<0||i>=rows)
        throw new OutOfBoundException("i",i,std::vector<int>(0,rows));
    T* res = (T*) malloc(columns*sizeof(T));
    for(int j=0;j<columns;j++)
        res[j] = data[columns*i+j];
    return res;
}

template<typename T>
T* Matrix<T>::getColumn(int j){
    if(j<0||j>=columns)
        throw new OutOfBoundException("j",j,std::vector<int>(0,columns));
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
Matrix<T>& Matrix<T>::operator*=(Matrix<T> const& other){
    if(columns!=other.rows)
        throw new IncompatibleDimensionsException(rows, columns,other.rows,other.columns,Operation::MULTIPLICATION);
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
        cblas_sgemm(CblasRowMajor,CblasNoTrans,CblasNoTrans,this->rows,other.columns,this->columns,1.0,reinterpret_cast<float*>(&data),this->columns,reinterpret_cast<float*>(&other.data),other.columns,0.0,reinterpret_cast<float*>(c_tab),other.columns);
    }
    else if(std::is_same<T,double>::value){
        cblas_dgemm(CblasRowMajor,CblasNoTrans,CblasNoTrans,this->rows,other.columns,this->columns,1.0,reinterpret_cast<double*>(&data),this->columns,reinterpret_cast<double*>(&other.data),other.columns,0.0,reinterpret_cast<double*>(c_tab),other.columns);
    }
    else{
        throw "Matrix multiplication is not supported for this type";
    }
    c.setData(c_tab);
    *this = c;
    return *this;
}

template<typename T>
void Matrix<T>::setData(T* data){
    for(int i=0;i<rows;i++)
        for(int j=0;j<columns;j++)
            this->data[columns*i+j] = data[columns*i+j];
}

template<typename T>
Matrix<T>& Matrix<T>::operator+=(Matrix<T> const& other){
    if(other.columns!=columns||other.rows!=rows)
        throw new IncompatibleDimensionsException(rows, columns,other.rows,other.columns,Operation::ADDITION);
    for (int i=0;i<rows;i++)
        for(int j=0;j<columns;j++)
            data[columns*i+j] += static_cast<T>(other.data[other.columns*i+j]);
    return *this;
}

template<typename T>
Matrix<T>& Matrix<T>::operator-=(Matrix<T> const& other){
    if(other.columns!=columns||other.rows!=rows)
        throw new IncompatibleDimensionsException(rows, columns,other.rows,other.columns,Operation::SUBTRACTION);
    for (int i=0;i<rows;i++)
        for(int j=0;j<columns;j++)
            data[columns*i+j] -= static_cast<T>(other.data[other.columns*i+j]);
    return *this;
}

template<typename T>
Matrix<T> Matrix<T>::operator*(Matrix<T> const& other){
    if(columns!=other.rows)
        throw new IncompatibleDimensionsException(rows, columns,other.rows,other.columns,Operation::MULTIPLICATION);
    Matrix res = *this;
    return res *= other;
}

template<typename T>
Matrix<T> Matrix<T>::operator+(Matrix<T> const& other){
    if(other.columns!=columns||other.rows!=rows)
        throw new IncompatibleDimensionsException(rows, columns,other.rows,other.columns,Operation::ADDITION);
    Matrix res = *this;
    return res += other;
}

//Check for equal sizes
template<typename T>
Matrix<T> Matrix<T>::operator-(Matrix<T> const& other){
    if(other.columns!=columns||other.rows!=rows)
        throw new IncompatibleDimensionsException(rows, columns,other.rows,other.columns,Operation::SUBTRACTION);
    Matrix res = *this;
    return res -= other;
}

template<typename T>
bool Matrix<T>::operator==(Matrix<T> const& other) const{
    if(columns!=other.columns||rows!=other.rows) return false;
    for(int i=0;i<rows;i++)
        for(int j=0;j<columns;j++)
            if(data[columns*i+j]!=other.data[other.columns*i+j]) return false;
    return true;
}

template<typename T>
bool Matrix<T>::operator!=(Matrix<T> const& other) const{
    return !(*this==other);
}

template<typename T>
T* Matrix<T>::operator[](const int &index) {
    return &data.data()[index * columns];
}

template<typename T>
void Matrix<T>::show() {
    std::cout << toString() << std::endl;
}

template<typename T>
std::string Matrix<T>::toString() {
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
std::map<EdgeHandling, std::function< Matrix<T> (Matrix<T> matrix, Matrix<M> mask, ImagePostProcess postProcess) >> Matrix<T>::edgeHandlingMethods = {
        {EdgeHandling::Extend, Matrix<T>::extendConvolve<M>},
        {EdgeHandling::Wrap, Matrix<T>::wrapConvolve<M>},
        {EdgeHandling::Mirror, Matrix<T>::mirrorConvolve<M>},
        {EdgeHandling::Crop, Matrix<T>::cropConvolve<M>},
        {EdgeHandling::KernelCrop, Matrix<T>::kernelCropConvolve<M>}
};

template<typename T>
template<typename M>
Matrix<T> Matrix<T>::extendConvolve(Matrix<T> matrix, Matrix<M> mask, ImagePostProcess postProcess) {
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
Matrix<T> Matrix<T>::wrapConvolve(Matrix<T> matrix, Matrix<M> mask, ImagePostProcess postProcess) {
    throw "Not implemented yet!";
}

template<typename T>
template<typename M>
Matrix<T> Matrix<T>::mirrorConvolve(Matrix<T> matrix, Matrix<M> mask, ImagePostProcess postProcess) {
    throw "Not implemented yet!";
}

template<typename T>
template<typename M>
Matrix<T> Matrix<T>::cropConvolve(Matrix<T> matrix, Matrix<M> mask, ImagePostProcess postProcess) {
//    printf("Function cropConvolve\n");
    // Compute the mask mean
    float mean = 0;
    for (int i = 0; i < mask.getElements(); i++) {
//        printf("Data : %d\n", mask.getData()[i]);
        mean += (float) mask.getData()[i];
    }
//    printf("Mask sum : %f\n", mean);
    // Compute the mask offset
    int offset = std::floor((double) mask.getRows() / 2.0);
    // Create the interMatrix matrix
    Matrix<int> interMatrix(matrix.rows - offset * 2, matrix.columns - offset * 2, MatrixArchetype::ZEROS);
//    Matrix<T> interMatrix(matrix);
    // Create the accumulator
    int accumulator;
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
//                    printf("accumulator + (matrixValue * maskValue) = %d + (%d * %f) =", accumulator, matrixValue, maskValue);
                    accumulator += (matrixValue * maskValue);
//                    printf("%d\n", accumulator);
                }
            }

            // Update the interMatrix matrix's pixel
            float temp = mean != 0 ? accumulator / mean : accumulator;
            minValue = fmin(minValue, temp);
            maxValue = fmax(maxValue, temp);
            interMatrix[row][col] = temp;
//            printf("Mean : %f, Accumulator : %d, Temp : %f, Min Value: %d, Max value : %d\n", mean, accumulator, temp, minValue, maxValue);
        }
    }
//    printf("Min Value: %d, Max value : %d\n", minValue, maxValue);

    // Clamp values to 0-255
    if (postProcess == ImagePostProcess::Clamp){
        for (int row = 0; row < interMatrix.getRows(); ++row)
        {
            for (int col = 0; col < interMatrix.getColumns(); ++col)
            {
//            printf("interMatrix[%d][%d] = ", interMatrix[row][col]);
                interMatrix[row][col] = std::clamp(interMatrix[row][col], 0, 255);
//                interMatrix[row][col] = interMatrix[row][col] < 255/2 ? 0 : 255;
//            printf("%d\n", interMatrix[row][col]);
            }
        }
    }

    // Normalise values to 0-255
    if (postProcess == ImagePostProcess::Normalize) {
        for (int row = 0; row < interMatrix.getRows(); ++row)
        {
            for (int col = 0; col < interMatrix.getColumns(); ++col)
            {
//                printf("interMatrix[%d][%d] = (%d - %d / %d) * 255 =", row, col, interMatrix[row][col], minValue, maxValue);
                if (maxValue == 0) maxValue = 1;
                interMatrix[row][col] = (int) (((float) (interMatrix[row][col] - minValue) / (float) maxValue) * 255);
//                printf("%d\n", interMatrix[row][col]);
            }
        }
    }

    Matrix<T> result(interMatrix);

    return result;
}

template<typename T>
template<typename M>
Matrix<T> Matrix<T>::kernelCropConvolve(Matrix<T> matrix, Matrix<M> mask, ImagePostProcess postProcess) {
    throw "Not implemented yet!";
}

template<typename T>
template<typename M>
Matrix<T> Matrix<T>::convolve(Matrix<M> const &mask, EdgeHandling edgeHandler, ushort nbIteration, ImagePostProcess postProcess) {
    Matrix convolution = edgeHandlingMethods<M>[edgeHandler](*this, mask, postProcess);
    for (int i = 1; i < nbIteration; ++i) {
        convolution = edgeHandlingMethods<M>[edgeHandler](convolution, mask, postProcess);
    }
    return convolution;
}

#endif