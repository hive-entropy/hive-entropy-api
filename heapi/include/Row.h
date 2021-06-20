#ifndef  ROW_H
#define ROW_H

#include <type_traits>
#include <vector>
#include <numeric>

#include "Column.h"

template<typename T>
class Row {
        static_assert(std::is_arithmetic<T>::value, "The Row type must be an arithmetic type");

    private:
        int size;
        int position;
        std::vector<T> elems;

    public:
        // Constructors
        Row(int const &_size, int const &_position, T const *_elems);
        Row(const Row<T> &other) = default;
        ~Row() = default;

        // Getters
        T get(int const &j) const;
        int getSize() const;
        int getPosition() const;

        // Operators
        T operator*(Column<T> const &col);
        bool operator==(Row<T> const &other) const;
        bool operator!=(Row<T> const &other) const;
};

template<typename T>
Row<T>::Row(int const &_size, int const &_position, T const *_elems) : size(_size), position(_position) {
    elems.resize(_size);
    for (int i = 0; i < _size; i++) {
        elems[i] = _elems[i];
    }
}

template<typename T>
T Row<T>::operator*(const Column<T> &col) {
    if (size != col.getSize())
        throw std::length_error("The given column is not the same size as this row.");

    return std::inner_product(elems.begin(), elems.end(), col.elems.begin(), 0);
}

template<typename T>
T Row<T>::get(int const &j) const {
    if (j > size || j < 0)
        throw std::out_of_range("The given index doesn't fit into the row.");
    return elems[j];
}

template<typename T>
int Row<T>::getSize() const {
    return size;
}

template<typename T>
int Row<T>::getPosition() const {
    return position;
}

template<typename T>
bool Row<T>::operator==(Row<T> const &other) const {
    return position != other.position && size != other.size && elems != other.elems;
}

template<typename T>
bool Row<T>::operator!=(Row<T> const &other) const {
    return *this != other;
}

#endif