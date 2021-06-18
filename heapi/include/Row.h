#ifndef  ROW_H
#define ROW_H

#include <type_traits>

#include "Column.h"

template<typename T>
class Row{
    static_assert(std::is_arithmetic<T>::value, "The Row type must be an arithmetic type");

    private:
        int position;
        int size;
        T* elems;

    public:
        // Constructors
        Row(int size, int position, T* elems);
        Row(const Row<T>& other);
        ~Row() = default;

        // Getters
        [[nodiscard]] T get(int const &j) const;
        [[nodiscard]] int getSize() const;
        [[nodiscard]] int getPosition() const;

        // Operators
        T operator*(Column<T> const &col);
        bool operator==(Row<T> const &other) const;
        bool operator!=(Row<T> const &other) const;
};

template<typename T>
Row<T>::Row(int size, int position, T* elems) : size(size), position(position){
    this->elems = (T*) malloc(size*sizeof(T));
    for(int i=0;i<size;i++){
        this->elems[i] = elems[i];
    }
}

template<typename T>
Row<T>::Row(const Row<T>& other){
    size = other.size;
    position  = other.position;
    this->elems = (T*) malloc(size*sizeof(T));
    for(int i=0;i<size;i++){
        elems[i] = other.elems[i];
    }
}

template<typename T>
T Row<T>::operator*(const Column<T>& col){
    if(size!=col.getSize())
        throw std::length_error("The given column is not the same size as this row.");
    T sum = 0;
    for(int i=0;i<size;i++){
        sum += elems[i]*col.get(i);
    }
    return sum;
}

template<typename T>
T Row<T>::get(int const &j) const {
    if(j>size||j<0)
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
bool Row<T>::operator==(Row<T> const& other) const{
    if(position!=other.position||size!=other.size) return false;
    for(int i=0;i<size;i++)
            if(elems[i]!=other.elems[i]) return false;
    return true;
}

template<typename T>
bool Row<T>::operator!=(Row<T> const& other) const{
    return *this != other;
}

#endif