#ifndef  ROW_H
#define ROW_H

template<typename T>
class Column;

#include "Column.h"

template<typename T>
class Row{

    static_assert(std::is_arithmetic<T>::value, "The Row type must be an arithmetic type");

    public:
        Row(int size, int position, T* elems);
        ~Row();
        Row(const Row<T>& other);
        T operator*(const Column<T>& col);

        T get(int j);
        int getSize();
        int getPosition();

    private:
        int position;
        int size;
        T* elems;
};

template<typename T>
Row<T>::Row(int size, int position, T* elems) : size(size), position(position){
    for(int i=0;i<size;i++){
        this->elems[i] = elems[i];
    }
}

template<typename T>
Row<T>::~Row(){

}

template<typename T>
Row<T>::Row(const Row<T>& other){
    size = other.size;
    position  = other.position;
    for(int i=0;i<size;i++){
        elems[i] = other.get(i);
    }
}

template<typename T>
T Row<T>::operator*(const Column<T>& col){
    if(size!=col.getSize())
        throw "Woops";
    T sum = 0;
    for(int i=0;i<size;i++){
        sum += elems[i]*col.get(i);
    }
    return sum;
}

template<typename T>
T Row<T>::get(int j){
    if(j>size||j<0)
        throw "Woops";
    return elems[j];
}

template<typename T>
int Row<T>::getSize(){
    return size;
}

template<typename T>
int Row<T>::getPosition(){
    return position;
}

#endif