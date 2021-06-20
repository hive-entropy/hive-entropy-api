#ifndef INCOMPATIBLE_DIMENSIONS_EXCEPTION
#define INCOMPATIBLE_DIMENSIONS_EXCEPTION

#include <iostream>

enum Operation{
            ADDITION,
            SUBTRACTION,
            MULTIPLICATION
};

class IncompatibleDimensionsException{
    private:
        int x1;
        int x2;
        int y1;
        int y2;
        Operation op;

    public:
        IncompatibleDimensionsException(int x1, int y1, int x2, int y2, Operation op) : x1(x1), x2(x2), y1(y1), y2(y2), op(op) {};
        friend std::ostream& operator<<(std::ostream& os, IncompatibleDimensionsException& ex);
};

#endif