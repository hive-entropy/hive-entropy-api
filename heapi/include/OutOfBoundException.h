#ifndef OUT_OF_BOUND_EXCEPTION_H
#define OUT_OF_BOUND_EXCEPTION_H

#include <iostream>
#include <vector>

class OutOfBoundException{
    public:
        OutOfBoundException(std::string var, int value,std::vector<int> bounds) : var(var),value(value), bounds(bounds){};
        friend std::ostream& operator<<(std::ostream& os, OutOfBoundException const& ex);
    private:
        std::string var;
        std::vector<int> bounds;
        int value;
};

#endif