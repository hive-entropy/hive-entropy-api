#include "catch.hpp"
#include "full.h"
#include <arpa/inet.h>

SCENARIO("We should be able to manipulate a message object","[message]"){
    GIVEN("A message object"){
        Message* mess = new Message();
        mess->setDest("coap://127.0.0.1:9999/");
        mess->setType(MessageType::NON_CONFIRMABLE);
        mess->setHttpMethod(HttpMethod::GET);
        mess->setContent("HolaBananezabonanana");
        mess->addHeader(Headers::PURPOSE,"banana");

        coap_session_t* sess;

        std::string destination = "coap://127.0.0.1:9999/";
        coap_uri_t* destParts = coap_new_uri(reinterpret_cast<const uint8_t*>(destination.c_str()),destination.length());
        coap_address_t dstAddr;

        coap_context_t* context;

        context = coap_new_context(NULL);

        coap_address_init(&dstAddr);
        dstAddr.addr.sin.sin_family = AF_INET;

        char uriBuf[destParts->host.length];
        strncpy(uriBuf,reinterpret_cast<const char*>(destParts->host.s),destParts->host.length);

        inet_pton(AF_INET, uriBuf, &dstAddr.addr.sin.sin_addr);
        dstAddr.addr.sin.sin_port = htons(destParts->port);

        sess = coap_new_client_session(context,NULL,&dstAddr,COAP_PROTO_UDP);

        WHEN("Convert it to a pdu and back"){
            coap_pdu_t* mess_coap = mess->toCoapMessage(sess);
            Message mess2(sess,mess_coap);

            THEN("Both messages have equal fields"){
                REQUIRE(mess->getContent()==mess2.getContent());
                REQUIRE(mess->getType()==mess2.getType());
                REQUIRE(mess->getHttpMethod()==mess2.getHttpMethod());
                for(std::pair<Headers,std::string> el : mess->getHeaders()){
                    REQUIRE(el.second==mess2.getHeaders()[el.first]);
                }
            }
        }
    }
}