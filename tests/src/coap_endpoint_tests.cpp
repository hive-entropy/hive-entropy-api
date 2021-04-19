#include "../include/catch.hpp"
#include "../../heapi/include/full.h"
#include <arpa/inet.h>

SCENARIO("We should be able to use the CoAP layer","[coap]"){
    GIVEN("A CoAP Endpoint"){
        CoapEndpoint e("coap://127.0.0.1:9669/");

        WHEN("We send a message to the listening server"){
            THEN("It throws no exception"){
                Message* mess = new Message();
                mess->setDest("coap://127.0.0.1:9999/");
                mess->setType(MessageType::NON_CONFIRMABLE);
                mess->setHttpMethod(HttpMethod::GET);
                mess->setContent("Hola");
                mess->addHeader(Headers::PURPOSE,"banana");

                e.send(*mess);
            }
        }
    }
}