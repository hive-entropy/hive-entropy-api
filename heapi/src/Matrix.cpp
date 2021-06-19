#include "Matrix.h"

/*/////////////////////////////////////
//  Template explicit instanciation  //
/////////////////////////////////////*/
template
class Matrix<int>;

template
class Matrix<float>;

template
class Matrix<double>;

template
class Matrix<unsigned short>;

template
class Matrix<unsigned char>;

/*/////////////////////
//  Implementations  //
/////////////////////*/

template<typename T>
void Matrix<T>::putColumn(int const &j, T const *elems) {
    if (j < 0 || j >= columns)
        throw OutOfBoundException("j", j, std::vector<int>(0, columns));
    for (int i = 0; i < rows; i++)
        data[columns * i + j] = elems[i];
}

template<typename T>
void Matrix<T>::putRow(int const &i, T const *elems) {
    if (i < 0 || i >= rows)
        throw OutOfBoundException("i", i, std::vector<int>(0, rows));
    for (int j = 0; j < columns; j++)
        data[columns * i + j] = elems[j];
}

template<typename T>
void Matrix<T>::put(int const &i, int const &j, T elem) {
    if (i < 0 || i >= rows)
        throw OutOfBoundException("i", i, std::vector<int>(0, rows));
    else if (j < 0 || j >= columns)
        throw OutOfBoundException("j", j, std::vector<int>(0, columns));
    data[columns * i + j] = elem;
}

template<typename T>
void Matrix<T>::putSubmatrix(int const &startRow, int const &startColumn, Matrix const &m) {
    if (startColumn + m.columns - 1 >= columns)
        throw OutOfBoundException("m.columns", m.columns, std::vector<int>(0, columns + 1 - startColumn));
    else if (startRow + m.rows - 1 >= rows)
        throw OutOfBoundException("m.rows", m.rows, std::vector<int>(0, rows - startRow + 1));
    for (int i = startRow; i < m.rows + startRow; i++) {
        for (int j = startColumn; j < m.columns + startColumn; j++) {
            data[columns * i + j] = static_cast<T>(m.data[m.columns * (i - startRow) + (j - startColumn)]);
        }
    }
}

template<typename T>
Matrix<T>
Matrix<T>::getSubmatrix(int const &startRow, int const &startColumn, int const &endRow, int const &endColumn) const {
    if (startColumn < 0 || startColumn >= columns || endColumn < startColumn || endColumn >= columns || startRow < 0 ||
        startRow >= rows || startRow > endRow || endRow >= rows || endRow < 0)
        throw "Whatever";
    Matrix<T> res(endRow - startRow + 1, endColumn - startColumn + 1);
    for (int i = startRow; i <= endRow; i++) {
        for (int j = startColumn; j <= endColumn; j++) {
            res.put(i - startRow, j - startColumn, data[columns * i + j]);
        }
    }
    return res;
}

template<typename T>
T Matrix<T>::get(int const &i, int const &j) const {
    if (i < 0 || i >= rows)
        throw OutOfBoundException("i", i, std::vector<int>(0, rows));
    else if (j < 0 || j >= columns)
        throw OutOfBoundException("j", j, std::vector<int>(0, columns));
    return data[columns * i + j];
}

template<typename T>
T *Matrix<T>::getRow(int const &i) const {
    if (i < 0 || i >= rows)
        throw OutOfBoundException("i", i, std::vector<int>(0, rows));
    T *res = (T *) malloc(columns * sizeof(T));
    for (int j = 0; j < columns; j++)
        res[j] = data[columns * i + j];
    return res;
}

template<typename T>
T *Matrix<T>::getColumn(int const &j) const {
    if (j < 0 || j >= columns)
        throw OutOfBoundException("j", j, std::vector<int>(0, columns));
    T *res = (T *) malloc(rows * sizeof(T));
    for (int i = 0; i < rows; i++)
        res[i] = data[columns * i + j];
    return res;
}

template<typename T>
T *Matrix<T>::getData() {
    return data.data();
}

template<typename T>
T const *Matrix<T>::getData() const {
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
Matrix<T> &Matrix<T>::operator*=(Matrix<T> const &other) {
    if (columns != other.rows)
        throw IncompatibleDimensionsException(rows, columns, other.rows, other.columns, Operation::MULTIPLICATION);
    Matrix<T> c(this->rows, other.columns, MatrixArchetype::ZEROS);
    T *c_tab = (T *) malloc(c.getRows() * c.getColumns() * sizeof(T));
    if (std::is_same<T, int>::value) {
        float *trailedData = (float *) malloc(elements * sizeof(float));
        float *trailedOther = (float *) malloc(other.elements * sizeof(float));
        float *trailedC_tab = (float *) malloc(c.getElements() * sizeof(float));
        for (int i = 0; i < elements; i++)
            trailedData[i] = static_cast<float>(data[i]);
        for (int i = 0; i < other.elements; i++)
            trailedOther[i] = static_cast<float>(other.data[i]);

        cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, this->rows, other.columns, this->columns, 1.0,
                    trailedData, this->columns, trailedOther, other.columns, 0.0, trailedC_tab, other.columns);

        for (int i = 0; i < c.getElements(); i++)
            c_tab[i] = static_cast<T>(trailedC_tab[i]);
    } else if (std::is_same<T, float>::value) {
        cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, this->rows, other.columns, this->columns, 1.0,
                    reinterpret_cast<const float *>(data.data()), this->columns,
                    reinterpret_cast<const float *>(other.data.data()), other.columns, 0.0,
                    reinterpret_cast<float *>(c_tab), other.columns);
    } else if (std::is_same<T, double>::value) {
        cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, this->rows, other.columns, this->columns, 1.0,
                    reinterpret_cast<const double *>(data.data()), this->columns,
                    reinterpret_cast<const double *>(other.data.data()), other.columns, 0.0,
                    reinterpret_cast<double *>(c_tab), other.columns);
    } else {
        throw "Matrix multiplication is not supported for this type";
    }
    c.setData(c_tab);
    *this = c;
    return *this;
}

template<typename T>
void Matrix<T>::setData(const T *data) {
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < columns; j++)
            this->data[columns * i + j] = data[columns * i + j];
}

template<typename T>
Matrix<T> &Matrix<T>::operator+=(Matrix<T> const &other) {
    if (other.columns != columns || other.rows != rows)
        throw IncompatibleDimensionsException(rows, columns, other.rows, other.columns, Operation::ADDITION);
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < columns; j++)
            data[columns * i + j] += static_cast<T>(other.data[other.columns * i + j]);
    return *this;
}

template<typename T>
Matrix<T> &Matrix<T>::operator-=(Matrix<T> const &other) {
    if (other.columns != columns || other.rows != rows)
        throw IncompatibleDimensionsException(rows, columns, other.rows, other.columns, Operation::SUBTRACTION);
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < columns; j++)
            data[columns * i + j] -= static_cast<T>(other.data[other.columns * i + j]);
    return *this;
}

template<typename T>
Matrix<T> Matrix<T>::operator*(Matrix<T> const &other) {
    if (columns != other.rows)
        throw IncompatibleDimensionsException(rows, columns, other.rows, other.columns, Operation::MULTIPLICATION);
    Matrix res = *this;
    return res *= other;
}

template<typename T>
Matrix<T> Matrix<T>::operator+(Matrix<T> const &other) {
    if (other.columns != columns || other.rows != rows)
        throw IncompatibleDimensionsException(rows, columns, other.rows, other.columns, Operation::ADDITION);
    Matrix res = *this;
    return res += other;
}

//Check for equal sizes
template<typename T>
Matrix<T> Matrix<T>::operator-(Matrix<T> const &other) {
    if (other.columns != columns || other.rows != rows)
        throw IncompatibleDimensionsException(rows, columns, other.rows, other.columns, Operation::SUBTRACTION);
    Matrix res = *this;
    return res -= other;
}

template<typename T>
bool Matrix<T>::operator==(Matrix<T> const &other) const {
    if (columns != other.columns || rows != other.rows) return false;
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < columns; j++)
            if (data[columns * i + j] != other.data[other.columns * i + j]) return false;
    return true;
}

template<typename T>
bool Matrix<T>::operator!=(Matrix<T> const &other) const {
    return !(*this == other);
}

template<typename T>
T *Matrix<T>::operator[](int const &index) {
    return &data.data()[index * columns];
}

template<typename T>
T const *Matrix<T>::operator[](int const &index) const {
    return &data.data()[index * columns];
}

template<typename T>
void Matrix<T>::show() const {
    std::cout << toString() << std::endl;
}

template<typename T>
std::string Matrix<T>::toString() const {
    std::string output;

    for (int i = 0; i < rows; i++) {
        output += "[ ";
        for (int j = 0; j < columns; j++) {
            output += std::to_string(data[columns * i + j]);
            output += " ";
        }
        output += "]\n";
    }

    return output;
}

template<typename T>
std::ostream &operator<<(std::ostream &os, Matrix<T> value) {
    os << value.toString();
    return os;
}