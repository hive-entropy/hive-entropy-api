#ifndef RESPONSE_BUILDER_H
#define RESPONSE_BUILDER_H

#include <string>
#include <vector>

#include "Message.h"
#include "Matrix.h"

using namespace std;

/**
 * @brief A ready-to-go class that builds common responses to incoming queries.
 * 
 */
class ResponseBuilder{
    public:
        /**
         * @brief Creates a response to confirm that the node is still alive.
         * 
         * @return Message the generated message.
         */
        static Message heartbeatMessage();

        /**
         * @brief Creates a response giving some info on the architecture, the IP address, and the potential proxies of the Node.
         * 
         * @return Message the generated message.
         */
        static Message identityMessage();

        /**
         * @brief Creates a response for a calculation assistance query.
         * 
         * @param answer true if the node will participate, otherwse false.
         * @return Message the generated message.
         */
        static Message assistanceResponseMessage(bool answer);

        /**
         * @brief Creates a message that gives a result matrix matrix fragment back to the querying node after a multiplication.
         * 
         * @tparam T the type of the matrix's elements.
         * @param calculationId the unique identifier for the calculation.
         * @param taskId the unique identifier for the task.
         * @param startRow the first row where the matrix fragment should start to be inserted.
         * @param startColumn the first column where the matrix fragment should start to be inserted.
         * @param fragment the matrix fragment to insert in the final result.
         * @return Message the generated message.
         */
        template<typename T>
        static Message matrixMultiplicationResultFragmentMessage(string calculationId, string taskId, int startRow, int startColumn, Matrix<T> fragment);

        /**
         * @brief Creates a message that gives a result matrix matrix fragment back to the querying node after a convolution.
         * 
         * @tparam T the type of the matrix's elements.
         * @param calculationId  the unique identifier for the calculation.
         * @param taskId the unique identifier for the task.
         * @param startRow the first row where the matrix fragment should start to be inserted.
         * @param endColumn the first column where the matrix fragment should start to be inserted.
         * @param fragment the matrix fragment to insert in the final result.
         * @return Message the generated message.
         */
        template<typename T>
        static Message matrixConvolutionResultFragmentMessage(string calculationId, string taskId, int startRow, int endColumn, Matrix<T> fragment);
};;

#endif