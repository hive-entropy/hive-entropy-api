#ifndef HIVE_ENTROPY_NODE_H
#define HIVE_ENTROPY_NODE_H

#include <functional>
#include <mutex>
#include <condition_variable>

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
        void sendMatrixMultiplicationTask(string uri, Matrix<T> a, Matrix<T> b, int insertX, int insertY, string calculationId);
        template<typename T>
        void sendMatrixMultiplicationTask(string uri,Row<T> row, Column<T> col, string calculationId);
        template<typename T>
        void sendMatrixConvolutionTask(string target, Matrix<T> a, Matrix<T> b, string calculationId, int borderSize);
        void sendHardwareSpecification(string uri);
        void sendAskingHardwareSpecification(string uri);
        void checkLiveness(string target);
        void queryNodeAvailability();
        void resolveNodeIdentities();

        void registerResponseHandler(coap_response_handler_t func);

        template<Message(*F)(Message)>
        void registerMessageHandler(string uri, HttpMethod method);

        template<Message(*F)(Message)>
        void registerAsynchronousHandler(string uri, HttpMethod method);

        void keepAlive();
    private:
        CoapEndpoint coap;
};

static std::map<coap_mid_t,std::mutex> locks = std::map<coap_mid_t,std::mutex>();
static std::map<coap_mid_t,std::condition_variable> cvs = std::map<coap_mid_t,std::condition_variable>();
static std::map<coap_mid_t,bool> canDie = std::map<coap_mid_t,bool>();

//-----------------------
//Templated methods

template<Message(*F)(Message)>
void HiveEntropyNode::registerMessageHandler(string uri, HttpMethod method) {
    coap_request_t coapMethod;

    switch (method){
        case HttpMethod::GET:
            coapMethod = COAP_REQUEST_GET;
            break;
        case HttpMethod::POST:
            coapMethod = COAP_REQUEST_POST;
            break;
        case HttpMethod::PUT:
            coapMethod = COAP_REQUEST_PUT;
            break;
        case HttpMethod::DELETE:
            coapMethod = COAP_REQUEST_DELETE;
            break;
        default:
            throw "Unknown HTTP Method";
    }

    coap.addResourceHandler(uri, coapMethod, [](coap_resource_t *resource, coap_session_t *session, const coap_pdu_t *request, const coap_string_t *query, coap_pdu_t *response){
        Message inputMessage(session, request);
        Message output = F(inputMessage);
        output.fillResponse(resource,session,request,response);
    });
};

template<Message(*F)(Message)>
void HiveEntropyNode::registerAsynchronousHandler(string uri, HttpMethod method){
    coap_request_t coapMethod;

    switch (method){
        case HttpMethod::GET:
            coapMethod = COAP_REQUEST_GET;
            break;
        case HttpMethod::POST:
            coapMethod = COAP_REQUEST_POST;
            break;
        case HttpMethod::PUT:
            coapMethod = COAP_REQUEST_PUT;
            break;
        case HttpMethod::DELETE:
            coapMethod = COAP_REQUEST_DELETE;
            break;
        default:
            throw "Unknown HTTP Method";
    }

    coap.addResourceHandler(uri, coapMethod, [](coap_resource_t *resource, coap_session_t *session, const coap_pdu_t *request, const coap_string_t *query, coap_pdu_t *response){
        if(request){
            coap_async_t *async;

            coap_bin_const_t token = coap_pdu_get_token(request);
            async = coap_find_async(session, token);

            if (!async) {
                async = coap_register_async(session, request,0);
                if (async == NULL) {
                    coap_pdu_set_code(response, COAP_RESPONSE_CODE_SERVICE_UNAVAILABLE);
                    return;
                }
                Message inputMessage(session, request);

                canDie[coap_pdu_get_mid(request)] = false; 
                std::thread t([](coap_session_t* sess, coap_bin_const_t* tok, Message input, coap_mid_t mid){
                    std::unique_lock<std::mutex> lock(locks[mid]);
                    coap_async_t* async = coap_find_async(sess, *tok);
                    canDie[mid] = true;
                    cvs[mid].notify_all();

                    Message* output  = new Message();
                    *output = F(input);
                    if(async){
                        coap_async_set_app_data(async,output);
                        coap_async_set_delay(async,1);
                    }
                }, session, &token, inputMessage,coap_pdu_get_mid(request));
                t.detach();
                coap_mid_t mid = coap_pdu_get_mid(request);
                std::unique_lock<std::mutex> lock(locks[coap_pdu_get_mid(request)]);
                cvs[mid].wait(lock,[mid]{
                    return canDie[mid];
                });
                return;
            }
        }

        coap_async_t *async;
        coap_bin_const_t token = coap_pdu_get_token(request);
        async = coap_find_async(session, token);

        if(async){
            Message* m = (Message*) coap_async_get_app_data(async);
            m->fillResponse(resource,session,request,response);
        }
        else{
            coap_pdu_set_code(response, COAP_RESPONSE_CODE_SERVICE_UNAVAILABLE);
            return;
        }
    });
}

template<typename T>
void HiveEntropyNode::sendMatrixMultiplicationTask(string uri, Matrix<T> a, Matrix<T> b, int insertX, int insertY, int steps, string taskId, string calculationId){
    Message m;

    if(uri.find("coap://")==std::string::npos)
        uri = "coap://"+uri;
    if(uri.find_last_of("/")!=uri.size()-1)
        uri +="/";

    m.setDest(uri+"task/multiplication/cannon");
    m.setHttpMethod(HttpMethod::POST);
    m.setType(MessageType::CONFIRMABLE);
    std::vector<Matrix<T>> vec;
    vec.push_back(a);
    vec.push_back(b);
    m.setContent(Serializer::serialize(vec));

    m.addHeader(Headers::SERIALIZED_TYPE,SERIALIZED_TYPE_MATRICES);
    m.addHeader(Headers::CALCULATION_ID,calculationId);
    m.addHeader(Headers::ELEMENT_TYPE,std::string(typeid(T).name()));
    m.addHeader(Headers::STEPS,std::to_string(steps));
    m.addHeader(Headers::TASK_ID,taskId);
    m.addHeader(Headers::INSERT_AT_X,to_string(insertX));
    m.addHeader(Headers::INSERT_AT_Y,to_string(insertY));

    send(m);
}

template<typename T>
void HiveEntropyNode::sendMatrixMultiplicationTask(string uri, Matrix<T> a, Matrix<T> b, int insertX, int insertY, string calculationId){
    Message m;

    if(uri.find("coap://")==std::string::npos)
        uri = "coap://"+uri;
    if(uri.find_last_of("/")!=uri.size()-1)
        uri +="/";

    m.setDest(uri+"task/multiplication/rowcol");
    m.setHttpMethod(HttpMethod::POST);
    m.setType(MessageType::CONFIRMABLE);
    std::vector<Matrix<T>> vec;
    vec.push_back(a);
    vec.push_back(b);
    m.setContent(Serializer::serialize(vec));

    m.addHeader(Headers::SERIALIZED_TYPE,SERIALIZED_TYPE_MATRICES);
    m.addHeader(Headers::CALCULATION_ID,calculationId);
    m.addHeader(Headers::ELEMENT_TYPE,std::string(typeid(T).name()));
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

    m.addHeader(Headers::SERIALIZED_TYPE,SERIALIZED_TYPE_ROWCOL);
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