#include "IncompatibleDimensionsException.h"

std::ostream& operator<<(std::ostream& os, IncompatibleDimensionsException& ex){
    switch(ex.op){
        case Operation::ADDITION:
            os << "Both operators should have the same dimension to perform an addition: ("<<ex.x1<<","<<ex.y1<<")!=("<<ex.x2<<","<<ex.y2<<")";
        break; 
        case Operation::SUBTRACTION:
            os << "Both operators should have the same dimension to perform a subtraction: ("<<ex.x1<<","<<ex.y1<<")!=("<<ex.x2<<","<<ex.y2<<")";
        break;
        case Operation::MULTIPLICATION:
            os << "The row count of the first operator is not equal to the columns of the second operator which is required to perform a multiplication: "<<ex.x1<<"!="<<ex.y2;
        break;
    }
    return os;
}