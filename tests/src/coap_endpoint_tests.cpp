#include "../include/catch.hpp"
#include "../../heapi/include/full.h"

SCENARIO("We should be able to use the CoAP layer","[coap]"){
    GIVEN("A CoAP Endpoint"){
        CoapEndpoint e("coap://127.0.0.1:9669/");

        WHEN("We send a message to the listening server"){
            THEN("We check"){
                Message* mess = new Message();
                mess->setDest("coap://127.0.0.1:9999/");
                mess->setType(Message::NON_CONFIRMABLE);
                mess->setHttpMethod(Message::GET);
                mess->setContent("Hola");

                e.send(*mess);
            }
        }
    }
}