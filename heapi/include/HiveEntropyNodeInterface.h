#ifndef HIVE_ENTROPY_NODE_INTERFACE_H
#define HIVE_ENTROPY_NODE_INTERFACE_H

#include <string>
#include <vector>
#include <future>

#include "Matrix.h"
#include "CoapEndpoint.h"
#include "Message.h"

using namespace std;

/**
 * @brief Interface representation of a Node (Worker or Captain) in the Hive Entropy network.
 */
class HiveEntropyNodeInterface{
    public:
        
        /**
         * @brief Constructor for the Node.
         *
         * @param uri The uri to use as a root for this Node.
        */
        HiveEntropyNodeInterface(string uri);

        /**
         * @brief Destructor for the Node.
         */
        ~HiveEntropyNodeInterface();

        /**
         * @brief Used to send a Message on the network. 
         *
         * Intended to be used by users that master the application-level protocol.
         * @param message The message object to be sent.
         */
        virtual void send(Message message);

        /**
         * @brief Sends a Matrix multiplication packet to calculate a result fragment on a Node.
         * 
         * @tparam T The type of the data contained in the matrix.
         * @param uri The address URI of the destination of the message.
         * @param a The first matrix fragment of the multiplication.
         * @param b The second matrix fragment of the multiplication.
         * @param steps The number of steps required to complete the calculation.
         * @param taskId The unique identifier of the calculation for this node.
         * @param calculationId The unique identifier for the multiplication.
         */
        template<typename T>
        void sendMatrixMultiplicationTask(string uri, Matrix<T> a, Matrix<T> b, int steps, string taskId, string calculationId);

        /**
         * @brief Sends a Matrix convolution packet to calculate a result fragment on a Node.
         * 
         * @tparam T The type of the data contained in the matrix.
         * @param uri The address URI of the destination of the message.
         * @param a The first matrix fragment of the convolution.
         * @param b The second matrix fragment of the convolution.
         * @param steps The number of steps required to complete the calculation.
         * @param taskId The unique identifier of the calculation for this node.
         * @param calculationId The unique identifier for the multiplication.
         * @param borderSize The size of the border to account for the shrinking.
         */
        template<typename T>
        void sendMatrixConvolutionTask(string uri, Matrix<T> a, Matrix<T> b, int steps, string taskId, string calculationId, int borderSize);

        /**
         * @brief Sends a liveness check message on the network, to know if a node is down or not.
         * 
         * @param uri The address URI of the destination of the message.
         * @return future<bool> A future for the value of the liveness of the node.
         */
        virtual void checkLiveness(string uri);

        /**
         * @brief Sends a broadcast query to get the availability of nodes to do calculations. Its response is to be interpreted as an agreement to participate in a calculation.
         */
        virtual void queryNodeAvailability();

        /**
         * @brief Queries information about nodes such as architecture, address, related proxy.
         */
        virtual void resolveNodeIdentities();

        /**
         * @brief Registers a callback to be executed at the reception of a response to an emitted query. As many callbacks as necessary can be created.
         * 
         * @param key A name to identify the callback.
         * @param func The callback function to execute, where the response message object is integrated.
         */
        virtual void registerResponseHandler(string key, void  (*func)(Message message));

         /**
         * @brief Registers a callback to be executed at the reception of a new message. As many callbacks as necessary can be created.
         * 
         * @param key A name to identify the callback.
         * @param func The callback function to execute, where the incoming message object is integrated. This method returns the response to be sent to the expeditor, or null if the message is to be ignored.
         */
        virtual void registerMessageHandler(string key, Message  (*func)(Message message));

    private:
        string uri;
        vector<string,void  (*)(Message message)> responseHandlers;
        vector<string,Message  (*)(Message message)> messageHandlers;
        CoapEndpoint coap;
};

#endif