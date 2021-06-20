#ifndef OUT_OF_BOUND_EXCEPTION_H
#define OUT_OF_BOUND_EXCEPTION_H

#include <iostream>
#include <vector>

class OutOfBoundException{
    private:
        int value;
        std::string var;
        std::vector<int> bounds;

    public:
        OutOfBoundException(int const &value, std::string const &var, std::vector<int> const &bounds)
                : value(value), var(var), bounds(bounds){};
        friend std::ostream& operator<<(std::ostream& os, OutOfBoundException const& ex);
};

#endif