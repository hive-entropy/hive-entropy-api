#ifndef RESPONSE_BUILDER_H
#define RESPONSE_BUILDER_H

#include <string>
#include <vector>

#include "Message.h"
#include "Matrix.h"
#include "Serializer.h"

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
        static Message hardwareMessage();

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
         * @brief Creates a message that gives a computed element after row column multiplication.
         * 
         * @tparam T the type of the element.
         * @param calculationId the unique identifier for the calculation.
         * @param insertX the insertion row.
         * @param insertY the insertion column. 
         * @param element the element to insert in the result.
         * @return Message the generated message. 
         */
        template<typename T>
        static Message matrixMultiplicationResultFragmentMessage(string calculationId, int insertX, int insertY, T element);

        /**
         * @brief Creates a message that gives a computed matrix after row column multiplication.
         * 
         * @tparam T the type of the matrix's element.
         * @param calculationId the unique identifier for the calculation.
         * @param insertX the insertion row.
         * @param insertY the insertion column. 
         * @param m the matrix to insert in the result.
         * @return Message the generated message. 
         */
        template<typename T>
        static Message matrixMultiplicationResultFragmentMessage(string calculationId, int insertX, int insertY, Matrix<T> m);

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
};

template<typename T>
Message ResponseBuilder::matrixMultiplicationResultFragmentMessage(string calculationId, string taskId, int startRow, int startColumn, Matrix<T> fragment){
    Message m;

    m.setType(MessageType::CONFIRMABLE);
    m.setHttpMethod(HttpMethod::OK);

    m.setContent(Serializer::serialize(fragment));

    m.addHeader(Headers::CALCULATION_ID,calculationId);
    m.addHeader(Headers::TASK_ID,taskId);
    m.addHeader(Headers::PURPOSE,PURPOSE_RESULT);
    m.addHeader(Headers::SERIALIZED_TYPE,SERIALIZED_TYPE_MATRIX);
    m.addHeader(Headers::ELEMENT_TYPE,typeid(T).name());
    m.addHeader(Headers::INSERT_AT_X,std::to_string(startRow));
    m.addHeader(Headers::INSERT_AT_Y,std::to_string(startColumn));

    return m;
}

template<typename T>
Message ResponseBuilder::matrixMultiplicationResultFragmentMessage(string calculationId, int insertX, int insertY, Matrix<T> mat){
    Message m;

    m.setType(MessageType::CONFIRMABLE);
    m.setHttpMethod(HttpMethod::OK);

    m.setContent(Serializer::serialize(mat));

    m.addHeader(Headers::CALCULATION_ID,calculationId);
    m.addHeader(Headers::SERIALIZED_TYPE,SERIALIZED_TYPE_MATRIX);
    m.addHeader(Headers::PURPOSE,PURPOSE_RESULT);
    m.addHeader(Headers::ELEMENT_TYPE,typeid(T).name());
    m.addHeader(Headers::INSERT_AT_X,std::to_string(insertX));
    m.addHeader(Headers::INSERT_AT_Y,std::to_string(insertY));

    return m;
}

template<typename T>
Message ResponseBuilder::matrixMultiplicationResultFragmentMessage(string calculationId, int insertX, int insertY, T element){
    Message m;

    m.setType(MessageType::CONFIRMABLE);
    m.setHttpMethod(HttpMethod::OK);

    m.setContent(std::to_string(element));

    m.addHeader(Headers::CALCULATION_ID,calculationId);
    m.addHeader(Headers::SERIALIZED_TYPE,SERIALIZED_TYPE_ELEMENT);
    m.addHeader(Headers::PURPOSE,PURPOSE_RESULT);
    m.addHeader(Headers::ELEMENT_TYPE,typeid(T).name());
    m.addHeader(Headers::INSERT_AT_X,std::to_string(insertX));
    m.addHeader(Headers::INSERT_AT_Y,std::to_string(insertY));

    return m;
}

template<typename T>
Message ResponseBuilder::matrixConvolutionResultFragmentMessage(string calculationId, string taskId, int startRow, int endColumn, Matrix<T> fragment){
    //Patience, you fool
}

#endif