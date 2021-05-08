#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include <coap2/coap.h>

class MessageHandler
{
private:
    /* data */
public:
    MessageHandler();
    ~MessageHandler();

    static coap_response_t handleMessage(coap_context_t *context, coap_session_t *session, coap_pdu_t *sent, coap_pdu_t *received, const coap_mid_t id);
};



#endif