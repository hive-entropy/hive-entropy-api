#include "MessageHandler.h"
#include "Message.h"
#include "Serializer.h"
#include "GlobalContext.h"

#include <string>
#include <iostream>

MessageHandler::MessageHandler()
{
}

MessageHandler::~MessageHandler()
{
}
/*
coap_response_t MessageHandler::handleMessage(coap_context_t *context, 
                                    coap_session_t *session, 
                                    coap_pdu_t *sent, 
                                    coap_pdu_t *received, 
                                    const coap_mid_t id){
    Message m(session,received);

    cout << "in handle message" << endl;
    
    std::string purpose = m.getHeaders()[Headers::SERIALIZED_TYPE];
    cout << "purpose value : " << purpose << endl;
    cout << "content value : " << m.getContent() << endl;
    printf("purpose : %s\n", purpose);
    printf("content : %s\n", m.getContent());
    return COAP_RESPONSE_OK;
}
*/

