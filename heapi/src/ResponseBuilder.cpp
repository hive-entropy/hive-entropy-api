#include "ResponseBuilder.h"

Message ResponseBuilder::heartbeatMessage(){
    Message m;
    m.setType(MessageType::ACK);
    m.setHttpMethod(HttpMethod::OK);

    return m;
}

Message ResponseBuilder::hardwareMessage(){
    Message m;
    m.setType(MessageType::ACK);
    m.setHttpMethod(HttpMethod::OK);

    m.setContent("No info for now");

    return m;
}

static Message assistanceResponseMessage(bool answer){
    Message m;
    m.setType(MessageType::NON_CONFIRMABLE);
    m.setHttpMethod(HttpMethod::OK);

    m.addHeader(Headers::ASSISTANCE_RESPONSE,std::to_string(answer));

    return m;
}

template<typename T>
static Message matrixMultiplicationResultFragmentMessage(string calculationId, string taskId, int startRow, int startColumn, Matrix<T> fragment){
    Message m;

    m.setType(MessageType::CONFIRMABLE);
    m.setHttpMethod(HttpMethod::OK);

    m.setContent(Serializer::serialize(fragment));

    m.addHeader(Headers::CALCULATION_ID,calculationId);
    m.addHeader(Headers::TASK_ID,taskId);
    m.addHeader(Headers::SERIALIZED_TYPE,"matrix");
    m.addHeader(Headers::INSERT_AT_X,std::to_string(startRow));
    m.addHeader(Headers::INSERT_AT_Y,std::to_string(startColumn));

    return m;
}

template<typename T>
static Message matrixMultiplicationResultFragmentMessage(string calculationId, int insertX, int insertY, T element){
    Message m;

    m.setType(MessageType::CONFIRMABLE);
    m.setHttpMethod(HttpMethod::OK);

    m.setContent(std::to_string(element));

    m.addHeader(Headers::CALCULATION_ID,calculationId);
    m.addHeader(Headers::SERIALIZED_TYPE,"element");
    m.addHeader(Headers::INSERT_AT_X,std::to_string(insertX));
    m.addHeader(Headers::INSERT_AT_Y,std::to_string(insertY));

    return m;
}

template<typename T>
static Message matrixConvolutionResultFragmentMessage(string calculationId, string taskId, int startRow, int endColumn, Matrix<T> fragment){

}