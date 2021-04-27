#include "ResponseBuilder.h"

Message ResponseBuilder::heartbeatMessage(){
    Message m;
    m.setType(MessageType::ACK);
    m.setHttpMethod(HttpMethod::OK);
    m.addHeader(Headers::PURPOSE,"health");

    return m;
}

Message ResponseBuilder::hardwareMessage(){
    Message m;
    m.setType(MessageType::ACK);
    m.setHttpMethod(HttpMethod::OK);
    m.addHeader(Headers::PURPOSE,"hardware");

    m.setContent("No info for now");

    return m;
}

static Message assistanceResponseMessage(bool answer){
    Message m;
    m.setType(MessageType::NON_CONFIRMABLE);
    m.setHttpMethod(HttpMethod::OK);
    m.addHeader(Headers::PURPOSE,"assistance");

    m.addHeader(Headers::ASSISTANCE_RESPONSE,std::to_string(answer));

    return m;
}