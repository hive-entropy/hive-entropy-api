#ifndef  ROW_H
#define ROW_H

template<typename T>
class Row;

#include "Row.h"

template<typename T>
class Column{

    static_assert(std::is_arithmetic<T>::value, "The Column type must be an arithmetic type");

    public:
        Column(int size, int position, T* elems);
        ~Column();
        Column(const Column<T>& other);
        T operator*(const Row<T>& col);

        T get(int j);
        int getSize();
        int getPosition();

    private:
        int position;
        int size;
        T* elems;
};

template<typename T>
Column<T>::Column(int size, int position, T* elems) : size(size), position(position){
    for(int i=0;i<size;i++){
        this->elems[i] = elems[i];
    }
}

template<typename T>
Column<T>::~Column(){

}

template<typename T>
Column<T>::Column(const Column<T>& other){
    size = other.size;
    position  = other.position;
    for(int i=0;i<size;i++){
        elems[i] = other.get(i);
    }
}

template<typename T>
T Column<T>::operator*(const Row<T>& col){
    if(size!=col.getSize())
        throw "Woops";
    T sum = 0;
    for(int i=0;i<size;i++){
        sum += elems[i]*col.get(i);
    }
    return sum;
}

template<typename T>
T Column<T>::get(int j){
    if(j>size||j<0)
        throw "Woops";
    return elems[j];
}

template<typename T>
int Column<T>::getSize(){
    return size;
}

template<typename T>
int Column<T>::getPosition(){
    return position;
}

#endif