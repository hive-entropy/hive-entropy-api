#include "HiveEntropyNode.h"
#include "GlobalContext.h"

HiveEntropyNode::HiveEntropyNode(std::string const &uri) : coap(uri) {}

void HiveEntropyNode::send(Message const &m){
    coap.send(m);
}

void HiveEntropyNode::checkLiveliness(std::string target){
    Message m;

    if(target.find("coap://") == std::string::npos)
        target = "coap://" + target;
    if(target.find_last_of('/') != target.size() - 1)
        target +="/";
        
    m.setDest(target + "health");
    m.setHttpMethod(HttpMethod::GET);
    m.setType(MessageType::NON_CONFIRMABLE);

    send(m);
}

void HiveEntropyNode::queryNodeAvailability(){
    Message m;
    m.setDest("coap://239.0.0.1:9999/require-help");
    m.setHttpMethod(HttpMethod::GET);
    m.setType(MessageType::NON_CONFIRMABLE);

    send(m);
}

void HiveEntropyNode::resolveNodeIdentities(){
    Message m;
    m.setDest("coap://239.0.0.1:9999/hardware");
    m.setHttpMethod(HttpMethod::GET);
    m.setType(MessageType::NON_CONFIRMABLE);
    m.addHeader(Headers::PURPOSE,PURPOSE_HARDWARE);

    send(m);
}

void HiveEntropyNode::registerResponseHandler(coap_response_handler_t const &func){
    coap.registerResponseHandler(func);
}

void HiveEntropyNode::keepAlive(){
    coap.waitForDeath();
}

void HiveEntropyNode::sendHardwareSpecification(std::string uri){
    Message m;

    if(uri.find("coap://")==std::string::npos)
        uri = "coap://"+uri;
    if(uri.find_last_of('/')!=uri.size()-1)
        uri +="/";

    m.setDest(uri+"hardware");
    m.setHttpMethod(HttpMethod::POST);
    m.setType(MessageType::CONFIRMABLE);
    m.addHeader(Headers::PURPOSE,PURPOSE_HARDWARE);
    send(m);
}

void HiveEntropyNode::sendAskingHardwareSpecification(std::string uri){
    Message m;

    if(uri.find("coap://")==std::string::npos)
        uri = "coap://"+uri;
    if(uri.find_last_of('/')!=uri.size()-1)
        uri +="/";

    m.setDest(uri+"hardware");
    m.setHttpMethod(HttpMethod::GET);
    m.setType(MessageType::CONFIRMABLE);
    m.addHeader(Headers::PURPOSE,PURPOSE_HARDWARE);
    send(m);
}