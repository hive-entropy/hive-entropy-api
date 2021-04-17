#include "OutOfBoundException.h"

std::ostream& operator<<(std::ostream& os, OutOfBoundException const& ex){
    os << "A parameter was out of the bounds of this Matrix: " << ex.var << "=" << ex.value << " but should be between " << ex.bounds.at(0) << " and " << ex.bounds.at(1);
    return os;
}