#include <iostream>
#include <string>

#include "Matrix.h"

using namespace std;

int main(){
    int tab[] = {1,2,3,4,5,6,7,8,9};
    Matrix<int> a(3,3,tab);

    char tab2[] = {'a','b','c','d','e','f','g','h','i'};
    Matrix<char> b(3,3,tab2);
    
    return 0;
}