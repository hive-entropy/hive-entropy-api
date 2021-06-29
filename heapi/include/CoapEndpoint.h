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
        /**
         * @brief The address structure of the local node.
         * 
         */
        coap_address_t localAddress{};

        /**
         * @brief The local URI oh the address, in the format of "coap://address:port/".
         */
        coap_uri_t* localRootUri;

        /**
         * @brief The list of resources (or paths) possessing a registered callback.
         * 
         */
        std::map<std::pair<std::string,coap_request_t>,coap_resource_t*> registeredResources;

        /**
         * @brief The mutex used to make modifications of the context object thread-safe.
         * 
         */
        std::mutex contextLock;

        /**
         * @brief The network loop thread, running the run() method.
         * 
         */
        std::thread loop;

        /**
         * @brief Toggle variable to signal to the network loop if it should continue reading or start shutdown.
         * 
         */
        bool keepAlive;

        /**
         * @brief The network loop method, used to read and write to the network.
         * 
         */
        void run() const;

    public:
        /**
         * @brief The coap context reference.
         * 
         */
        coap_context_t *context;

        /**
         * @brief Constructor for CoapEndpoint.
         * 
         * @param rootUri The root URI to use as a base for the node to be contacted (resources are paths that descend from this URI).
         */
        explicit CoapEndpoint(std::string rootUri);

        /**
         * @brief Destructor for CoapEndpoint.
         * 
         */
        ~CoapEndpoint();

        /**
         * @brief Sends a message over the network.
         * 
         * @param m The Message to send.
         */
        void send(Message m);

        /**
         * @brief Adds a resource handling callback method for a combo of a resource path and HTTP-like method.
         * 
         * @param path The path of the resource to handle.
         * @param method The HTTP-like method (GET, POST, PUT or DELETE)
         * @param h The handler method, following libcoap specifications.
         */
        void addResourceHandler(std::string const &path, coap_request_t const &method, coap_method_handler_t const &h);

        /**
         * @brief Registers a response handler for the node.
         * 
         * @param h The response handler method in the libcoap specification.
         */
        void registerResponseHandler(coap_response_handler_t const &h) const;

        /**
         * @brief Deregisters the current response handler for the node.
         * 
         */
        void unregisterResponseHandler() const;

        /**
         * @brief Deregisters the current handler callback for the specified resource path and HTTP-like method.
         * 
         * @param path The Resource path to deregister.
         * @param method The HTTP-like method for which the resource will be deregistered.
         */
        void unregisterResourceHandler(std::string const &path, coap_request_t const &method);

        /**
         * @brief Waits for the network thread to die before returning.
         * 
         * Used in the destructor to not leave a trailing reference to this, and is called after passing keepAlive to false.
         * 
         */
        void waitForDeath();
};

#endif