#include "HiveEntropyNode.h"
#include "Serializer.h"
#include "GlobalContext.h"

HiveEntropyNode::HiveEntropyNode(std::string uri) : /*HiveEntropyNodeInterface(uri),*/ coap(uri){}

HiveEntropyNode::~HiveEntropyNode(){
    //delete &coap;
}

void HiveEntropyNode::send(Message m){
    coap.send(m);
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

void HiveEntropyNode::registerMessageHandler(string key, HttpMethod method, coap_method_handler_t func){
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

    coap.addResourceHandler(key, coapMethod, func);
}

void HiveEntropyNode::keepAlive(){
    coap.waitForDeath();
};