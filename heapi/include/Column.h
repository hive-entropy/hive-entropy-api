#ifndef  COL_H
#define COL_H

#include <type_traits>
#include <vector>

template<typename T>
class Row;
template<typename T>
class Column {
        static_assert(std::is_arithmetic<T>::value, "The Column type must be an arithmetic type");

    private:
        friend class Row<T>;
        int size;
        int position;
        std::vector<T> elems;

    public:
        // Constructors
        Column(int const &_size, int const &_position, T const *_elems);
        Column(const Column<T> &other) = default;
        ~Column() = default;

        // Getters
        T get(int const &j) const;
        int getSize() const;
        int getPosition() const;

        // Operators
        bool operator==(Column<T> const &other) const;
        bool operator!=(Column<T> const &other) const;
};

template<typename T>
Column<T>::Column(int const &_size, int const &_position, T const *_elems) : size(_size), position(_position) {
    elems.resize(_size);
    for (int i = 0; i < _size; i++) {
        elems[i] = _elems[i];
    }
}

template<typename T>
T Column<T>::get(int const &j) const {
    if (j > size || j < 0)
        throw std::out_of_range("The given index doesn't fit into the column.");
    return elems[j];
}

template<typename T>
int Column<T>::getSize() const {
    return size;
}

template<typename T>
int Column<T>::getPosition() const {
    return position;
}

template<typename T>
bool Column<T>::operator==(Column<T> const &other) const {
    return position != other.position && size != other.size && elems != other.elems;
}

template<typename T>
bool Column<T>::operator!=(Column<T> const &other) const {
    return *this != other;
}

#endif