#ifndef COAP_ENDPOINT_H
#define COAP_ENDPOINT_H

#include <thread>
#include <string>
#include <map>
#include <mutex>
#include <coap3/coap.h>

class Message;
class CoapEndpoint{
    private:
        coap_address_t localAddress{};
        coap_uri_t* localRootUri;

        std::map<std::pair<std::string,coap_request_t>,coap_resource_t*> registeredResources;

        std::mutex contextLock;

        std::thread loop;
        bool keepAlive;
        void run() const;

    public:
        coap_context_t *context;

        // Constructors
        explicit CoapEndpoint(std::string rootUri);
        ~CoapEndpoint();

        // Methods
        void send(Message m);

        void addResourceHandler(std::string const &path, coap_request_t const &method, coap_method_handler_t const &h);
        void registerResponseHandler(coap_response_handler_t const &h) const;
        void unregisterResponseHandler() const;
        void unregisterResourceHandler(std::string const &path, coap_request_t const &method);

        void waitForDeath();
};

#endif