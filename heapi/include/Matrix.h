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
    None,
    Normalize,
    Clamp
};


/**
 * @brief .
 */
template<typename T>
class Matrix{
    static_assert(std::is_arithmetic<T>::value, "The Matrix type must be an arithmetic type");

    private:
        /**
         * @brief Number of matrix row
         */
        int rows;

        /**
         * @brief Number of matrix column
         */
        int columns;

        /**
         * @brief Number of matrix elements
         */
        int elements;

        /**
         * @brief Type of matrix's element
         */
        int type;

        /**
         * @brief Matrix datas
         */
        std::vector<T> data;


        template<typename M>
        /**
         * @brief Method chosen to handle matrix border in convolution
         *
         */
        static std::map<EdgeHandling, std::function< Matrix<T> (Matrix<T> const &matrix, Matrix<M> const &mask, ImagePostProcess const &postProcess) >> edgeHandlingMethods;

        template<typename M>
        /**
         * @brief Convolve matrix using mask and use the extend method to handle matrix borders
         *
         * @param matrix Matrix to convolve
         * @param mask Mask to apply
         * @param postProcess Method to use in post processing
         * @return Matrix<T>
         */
        static Matrix<T> extendConvolve(Matrix<T> const &matrix, Matrix<M> const &mask, ImagePostProcess const &postProcess);

        template<typename M>
        /**
         * @brief Convolve matrix using mask and use the wrap method to handle matrix borders
         *
         * @param matrix Matrix to convolve
         * @param mask Mask to apply
         * @param postProcess Method to use in post processing
         * @return Matrix<T>
         */
        static Matrix<T> wrapConvolve(Matrix<T> const &matrix, Matrix<M> const &mask, ImagePostProcess const &postProcess);

        template<typename M>
        /**
         * @brief Convolve matrix using mask and use the mirror method to handle matrix borders
         *
         * @param matrix Matrix to convolve
         * @param mask Mask to apply
         * @param postProcess Method to use in post processing
         * @return Matrix<T>
         */
        static Matrix<T> mirrorConvolve(Matrix<T> const &matrix, Matrix<M> const &mask, ImagePostProcess const &postProcess);

        template<typename M>
        /**
         * @brief Convolve matrix using mask and use the crop method to handle matrix borders
         *
         * @param matrix Matrix to convolve
         * @param mask Mask to apply
         * @param postProcess Method to use in post processing
         * @return Matrix<T>
         */
        static Matrix<T> cropConvolve(Matrix<T> const &matrix, Matrix<M> const &mask, ImagePostProcess const &postProcess);

        template<typename M>
        /**
         * @brief Convolve matrix using mask and use the kernel crop method to handle matrix borders
         *
         * @param matrix Matrix to convolve
         * @param mask Mask to apply
         * @param postProcess Method to use in post processing
         * @return Matrix<T>
         */
        static Matrix<T> kernelCropConvolve(Matrix<T> const &matrix, Matrix<M> const &mask, ImagePostProcess const &postProcess);

    public:
        // Constructors
        template<typename M>
        Matrix(int const &rows, int const &columns, M const *data);
        Matrix(int const &rows, int const &columns, char const &archetype= MatrixArchetype::ZEROS);
        template<typename M>
        explicit Matrix(Matrix<M> const &m);

        // Getters
        int getRows() const;
        int getColumns() const;
        int getType() const;
        int getElements() const;

        T get(int const &i, int const &j) const;
        T* getRow(int const &i) const;
        T* getColumn(int const &j) const;

        Matrix getSubmatrix(int const &startRow, int const &startColumn, int const &endRow, int const &endColumn) const;

        T* getData();
        T const* getData() const;

        T* operator[](int const &index);
        T const* operator[](int const &index) const;

        // Setters
        void put(int const &i, int const &j, T elem);
        void putRow(int const &i, T const *elems);
        void putColumn(int const &j, T const *elems);
        void putSubmatrix(int const &startRow, int const &startColumn, Matrix const &m);
        void setData(const T *data);

        // Operators
        Matrix operator*(Matrix<T> const &other);
        Matrix operator+(Matrix<T> const &other);
        Matrix operator-(Matrix<T> const &other);
        Matrix& operator*=(Matrix<T> const &other);
        Matrix& operator+=(Matrix<T> const &other);
        Matrix& operator-=(Matrix<T> const &other);
        bool operator==(Matrix<T> const &other) const;
        bool operator!=(Matrix<T> const &other) const;

        // Methods
        template<typename M>
        /**
         * @brief Convolve matrix with mask using parametters
         *
         * @param mask Mask to apply
         * @param edgeHandler Method used to handle edge
         * @param nbIteration Number of iteration
         * @param postProcess Method to use in post processing
         * @return Matrix
         */
        Matrix convolve(Matrix<M> const &mask, EdgeHandling const &edgeHandler = EdgeHandling::Extend, ushort const &nbIteration = 1, ImagePostProcess const &postProcess = ImagePostProcess::Normalize) const;

        // Output
        void show() const;
        std::string toString() const;
};


template<typename T>
template<typename M>
Matrix<T>::Matrix(int const &rows, int const &columns, M const *data) : rows(rows), columns(columns) {
    static_assert(std::is_arithmetic<M>::value, "The Matrix type must be an arithmetic type");
    elements = rows * columns;
    // Copy the input data to the intern data vector
    this->data.resize(elements);
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < columns; j++)
            this->data[columns * i + j] = static_cast<T>(data[columns * i + j]);
}

template<typename T>
Matrix<T>::Matrix(int const &rows, int const &columns, char const &archetype) : rows(rows), columns(columns) {
    elements = rows * columns;
    this->data.resize(elements);
    switch (archetype) {
        case MatrixArchetype::ZEROS:
            for (int i = 0; i < rows; i++)
                for (int j = 0; j < columns; j++)
                    data[columns * i + j] = static_cast<T>(0.0);
            break;
        case MatrixArchetype::ONES:
            for (int i = 0; i < rows; i++)
                for (int j = 0; j < columns; j++)
                    data[columns * i + j] = static_cast<T>(1.0);
            break;
        case MatrixArchetype::ID:
            for (int i = 0; i < rows; i++)
                for (int j = 0; j < columns; j++)
                    data[columns * i + j] = (i == j) ? static_cast<T>(1.0) : static_cast<T>(0.0);
            break;
    }
}

template<typename T>
template<typename M>
Matrix<T>::Matrix(Matrix<M> const &m) {
    static_assert(std::is_arithmetic<M>::value, "The Matrix type must be an arithmetic type");
    rows = m.getRows();
    columns = m.getColumns();
    elements = rows * columns;
    this->data.resize(elements);
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < columns; j++)
            data[columns * i + j] = static_cast<T>(m.getData()[m.getColumns() * i + j]);
}

/*////////////////////////////////
///     MATRIX CONVOLUTION     ///
////////////////////////////////*/

template<typename T>
template<typename M>
std::map<EdgeHandling, std::function<Matrix<T>(Matrix<T> const &matrix, Matrix<M> const &mask,
                                               ImagePostProcess const &postProcess) >> Matrix<T>::edgeHandlingMethods = {
        {EdgeHandling::Extend,     Matrix<T>::extendConvolve<M>},
        {EdgeHandling::Wrap,       Matrix<T>::wrapConvolve<M>},
        {EdgeHandling::Mirror,     Matrix<T>::mirrorConvolve<M>},
        {EdgeHandling::Crop,       Matrix<T>::cropConvolve<M>},
        {EdgeHandling::KernelCrop, Matrix<T>::kernelCropConvolve<M>}
};

template<typename T>
template<typename M>
Matrix<T>
Matrix<T>::extendConvolve(Matrix<T> const &matrix, Matrix<M> const &mask, ImagePostProcess const &postProcess) {
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
Matrix<T>
Matrix<T>::mirrorConvolve(Matrix<T> const &matrix, Matrix<M> const &mask, ImagePostProcess const &postProcess) {
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
                    T matrixValue = matrix[row + maskRow][col + maskCol];
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
    if (postProcess == ImagePostProcess::Clamp) {
        for (int row = 0; row < interMatrix.getRows(); ++row) {
            for (int col = 0; col < interMatrix.getColumns(); ++col) {
                interMatrix[row][col] = std::clamp(interMatrix[row][col], 0, 255);
            }
        }
    }

    // Normalise values to 0-255
    if (postProcess == ImagePostProcess::Normalize) {
        for (int row = 0; row < interMatrix.getRows(); ++row) {
            for (int col = 0; col < interMatrix.getColumns(); ++col) {
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
Matrix<T>
Matrix<T>::kernelCropConvolve(Matrix<T> const &matrix, Matrix<M> const &mask, ImagePostProcess const &postProcess) {
    throw "Not implemented yet!";
}

template<typename T>
template<typename M>
Matrix<T> Matrix<T>::convolve(Matrix<M> const &mask, EdgeHandling const &edgeHandler, ushort const &nbIteration,
                              ImagePostProcess const &postProcess) const {
    Matrix convolution = edgeHandlingMethods<M>[edgeHandler](*this, mask, postProcess);
    for (int i = 1; i < nbIteration; ++i) {
        convolution = edgeHandlingMethods<M>[edgeHandler](convolution, mask, postProcess);
    }
    return convolution;
}

#endif