#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include <coap2/coap.h>
#include "MessageHandler.h"
#include "Message.h"
#include "Serializer.h"
#include "GlobalContext.h"

#include <string>
#include <iostream>

class MessageHandler
{
private:
    /* data */
public:
    MessageHandler();
    ~MessageHandler();

    static coap_response_t handleMessage(coap_context_t *context, coap_session_t *session, coap_pdu_t *sent, coap_pdu_t *received, const coap_mid_t id)
    {
    Message m(session,received);

    cout << "in handle message" << endl;
    
    std::string purpose = m.getHeaders()[Headers::SERIALIZED_TYPE];
    cout << "purpose value : " << purpose << endl;
    cout << "content value : " << m.getContent() << endl;
    printf("purpose : %s\n", purpose);
    printf("content : %s\n", m.getContent());
    return COAP_RESPONSE_OK;
    }
  

};



#endif