#ifndef HIVE_ENTROPY_NODE_H
#define HIVE_ENTROPY_NODE_H

//#include "HiveEntropyNodeInterface.h"
#include "Matrix.h"
#include "Message.h"
#include "CoapEndpoint.h"
#include "Row.h"
#include "Column.h"
#include "Serializer.h"

class HiveEntropyNode /*:public HiveEntropyNodeInterface*/{
    public:
        HiveEntropyNode(std::string uri);
        ~HiveEntropyNode();

        void send(Message m);

        template<typename T>
        void sendMatrixMultiplicationTask(string target, Matrix<T> a, Matrix<T> b, int insertX, int insertY, int steps, string taskId, string calculationId);
        template<typename T>
        void sendMatrixMultiplicationTask(string target,Row<T> rows, Column<T> col, string calculationId);
        template<typename T>
        void sendMatrixConvolutionTask(string target, Matrix<T> a, Matrix<T> b, string calculationId, int borderSize);

        void checkLiveness(string target);
        void queryNodeAvailability();
        void resolveNodeIdentities();

        void registerResponseHandler(coap_response_handler_t func);
        void registerMessageHandler(string uri, HttpMethod method, coap_method_handler_t func);

        void keepAlive();
    private:
        CoapEndpoint coap;
        int banana=5;
};

template<typename T>
void HiveEntropyNode::sendMatrixMultiplicationTask(string uri, Matrix<T> a, Matrix<T> b, int insertX, int insertY, int steps, string taskId, string calculationId){
    Message m;

    if(uri.find("coap://")==std::string::npos)
        uri = "coap://"+uri;
    if(uri.find_last_of("/")!=uri.size()-1)
        uri +="/";

    m.setDest(uri+"task/multiplication/cannon");
    m.setHttpMethod(HttpMethod::POST);
    m.setType(MessageType::NON_CONFIRMABLE);
    std::vector<Matrix<T>> vec;
    vec.push_back(a);
    vec.push_back(b);
    m.setContent(Serializer::serialize(vec));

    m.addHeader(Headers::PURPOSE,"assistance");
    m.addHeader(Headers::SERIALIZED_TYPE,"matrices");
    m.addHeader(Headers::CALCULATION_ID,calculationId);
    m.addHeader(Headers::ELEMENT_TYPE,std::string(typeid(T).name()));
    m.addHeader(Headers::STEPS,std::to_string(steps));
    m.addHeader(Headers::TASK_ID,taskId);
    m.addHeader(Headers::INSERT_AT_X,to_string(insertX));
    m.addHeader(Headers::INSERT_AT_Y,to_string(insertY));

    send(m);
}

template<typename T>
void HiveEntropyNode::sendMatrixMultiplicationTask(string uri,Row<T> row, Column<T> col, string calculationId){
    Message m;

    if(uri.find("coap://")==std::string::npos)
        uri = "coap://"+uri;
    if(uri.find_last_of("/")!=uri.size()-1)
        uri +="/";

    m.setDest(uri+"task/multiplication/rowcol");
    m.setHttpMethod(HttpMethod::POST);
    m.setType(MessageType::CONFIRMABLE);
    m.setContent(Serializer::serialize(row,col));

    m.addHeader(Headers::PURPOSE,"assistance");
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

#endif