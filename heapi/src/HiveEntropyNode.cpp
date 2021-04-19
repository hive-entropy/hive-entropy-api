#include "HiveEntropyNode.h"
#include "Serializer.h"
#include "GlobalContext.h"

HiveEntropyNode::HiveEntropyNode(std::string uri) : HiveEntropyNodeInterface(uri), coap(uri){}

HiveEntropyNode::~HiveEntropyNode(){
    delete &coap;
}

void HiveEntropyNode::send(Message m){
    coap.send(m);
}

template<typename T>
void HiveEntropyNode::sendMatrixMultiplicationTask(string uri, Matrix<T> a, Matrix<T> b, int insertX, int insertY, int steps, string taskId, string calculationId){
    Message m;
    m.setDest(uri+"/task/multiplication/cannon");
    m.setHttpMethod(HttpMethod::POST);
    m.setType(MessageType::CONFIRMABLE);
    m.setContent(Serializer::serialize(std::vector<Matrix<T>>(a,b)));

    m.addHeader(Headers::SERIALIZED_TYPE,"matrices");
    m.addHeader(Headers::CALCULATION_ID,calculationId);
    m.addHeader(Headers::ELEMENT_TYPE,typeid(T).name());
    m.addHeader(Headers::STEPS,std::to_string(steps));
    m.addHeader(Headers::TASK_ID,taskId);
    m.addHeader(Headers::INSERT_AT_X,to_string(insertX));
    m.addHeader(Headers::INSERT_AT_Y,to_string(insertY));

    send(m);
}

template<typename T>
void HiveEntropyNode::sendMatrixMultiplicationTask(string uri,Row<T> row, Column<T> col, string calculationId){
    Message m;
    m.setDest(uri+"/task/multiplication/rowcol");
    m.setHttpMethod(HttpMethod::POST);
    m.setType(MessageType::CONFIRMABLE);
    m.setContent(Serializer::serialize(row,col));

    m.addHeader(Headers::SERIALIZED_TYPE,"rowcol");
    m.addHeader(Headers::CALCULATION_ID,calculationId);
    m.addHeader(Headers::ELEMENT_TYPE,typeid(T).name());
    m.addHeader(Headers::INSERT_AT_X,std::to_string(row.getPosition()));
    m.addHeader(Headers::INSERT_AT_Y,std::to_string(col.getPosition()));

    send(m);
}

template<typename T>
void HiveEntropyNode::sendMatrixConvolutionTask(string uri, Matrix<T> a, Matrix<T> b, string calculationId, int borderSize){
    //LATER, BE PATIENT ;p
}

void HiveEntropyNode::checkLiveness(string uri){
    Message m;
    m.setDest(uri+"/health");
    m.setHttpMethod(HttpMethod::GET);
    m.setType(MessageType::NON_CONFIRMABLE);

    send(m);
}

void HiveEntropyNode::queryNodeAvailability(){
    Message m;
    m.setDest("224.0.1.187/require-help");
    m.setHttpMethod(HttpMethod::GET);
    m.setType(MessageType::NON_CONFIRMABLE);

    send(m);
}

void HiveEntropyNode::resolveNodeIdentities(){
    Message m;
    m.setDest("224.0.1.187/hardware");
    m.setHttpMethod(HttpMethod::GET);
    m.setType(MessageType::NON_CONFIRMABLE);

    send(m);
}

void HiveEntropyNode::registerResponseHandler(coap_response_handler_t func){
    coap.registerResponseHandler(func);
}

void HiveEntropyNode::registerMessageHandler(string key, coap_request_t method, coap_method_handler_t func){
    coap.addResourceHandler(key, method, func);
}