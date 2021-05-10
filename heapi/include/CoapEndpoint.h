#ifndef COAP_ENDPOINT_H
#define COAP_ENDPOINT_H

#include <thread>
#include <string>
#include <map>

#include <coap2/coap.h>

#include "Message.h"

class CoapEndpoint{
    public:
        CoapEndpoint(std::string rootUri);
        ~CoapEndpoint();

        void addResourceHandler(std::string path, coap_request_t method, coap_method_handler_t h);
        void registerResponseHandler(coap_response_handler_t h);

        void unregisterResponseHandler();
        void unregisterResourceHandler(std::string path, coap_request_t method);

        void send(Message m);

        void endSession(string uri);

        void waitForDeath();
    private:
        coap_context_t *context;
        coap_address_t localAddress;
        coap_uri_t* localRootUri;

        std::map<std::pair<std::string,coap_request_t>,coap_resource_t*> registeredResources;
        std::map<std::string,coap_session_t*> activeSessions;
        coap_endpoint_t*  listener;

        std::thread loop;
        bool keepAlive;
        void run();
};

#endif