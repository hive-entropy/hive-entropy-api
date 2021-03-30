#ifndef RESPONSE_BUILDER_H
#define RESPONSE_BUILDER_H

#include <string>
#include <vector>

#include "Message.h"
#include "Matrix.h"

using namespace std;

class ResponseBuilder{
    public:
        static Message heartbeatMessage();
        static Message identityMessage(string uri);
        static Message assistanceResponseMessage(bool answer);

        template<typename T>
        static Message matrixMultiplicationResultFragmentMessage(string calculationId, string taskId, int startRow, int startColumn, Matrix<T> fragment);

        template<typename T>
        static Message matrixConvolutionResultFragmentMessage(string calculationId, string taskId, int startRow, int startColumn, Matrix<T> fragment);
};;

#endif