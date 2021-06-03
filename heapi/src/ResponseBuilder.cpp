#include "ResponseBuilder.h"
#include "hardware.h"

Message ResponseBuilder::heartbeatMessage(){
    Message m;
    m.setType(MessageType::ACK);
    m.setHttpMethod(HttpMethod::OK);
    m.addHeader(Headers::PURPOSE,PURPOSE_HEALTH);

    return m;
}

Message ResponseBuilder::hardwareMessage(){
    Message m;
    Hardware hw;
    m.setType(MessageType::ACK);
    m.setHttpMethod(HttpMethod::OK);
    m.addHeader(Headers::PURPOSE,PURPOSE_HARDWARE);

    m.setContent(hw.toString());

    return m;
}

Message ResponseBuilder::assistanceResponseMessage(bool answer){
    Message m;
    m.setType(MessageType::NON_CONFIRMABLE);
    m.setHttpMethod(HttpMethod::OK);
    m.addHeader(Headers::PURPOSE,PURPOSE_ASSISTANCE);
    m.addHeader(Headers::ASSISTANCE_RESPONSE,std::to_string(answer));

    return m;
}