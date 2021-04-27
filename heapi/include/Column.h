#ifndef  COL_H
#define COL_H

#include <type_traits>

template<typename T>
class Column{

    static_assert(std::is_arithmetic<T>::value, "The Column type must be an arithmetic type");

    public:
        Column(int size, int position, T* elems);
        ~Column();
        Column(const Column<T>& other);
        bool operator==(Column<T> const& other) const;
        bool operator!=(Column<T> const& other) const;

        T get(int j) const;
        int getSize() const;
        int getPosition() const;

    private:
        int position;
        int size;
        T* elems;
};

template<typename T>
Column<T>::Column(int size, int position, T* elems) : size(size), position(position){
    this->elems = (T*) malloc(size*sizeof(T));
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
    this->elems = (T*) malloc(size*sizeof(T));
    for(int i=0;i<size;i++){
        elems[i] = other.elems[i];
    }
}

template<typename T>
T Column<T>::get(int j) const {
    if(j>size||j<0)
        throw "Woops";
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
bool Column<T>::operator==(Column<T> const& other) const{
    if(position!=other.position||size!=other.size) return false;
    for(int i=0;i<size;i++)
            if(elems[i]!=other.elems[i]) return false;
    return true;
}

template<typename T>
bool Column<T>::operator!=(Column<T> const& other) const{
    return !(*this==other);
}

#endif