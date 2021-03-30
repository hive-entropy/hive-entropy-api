#ifndef HIVE_ENTROPY_NODE_INTERFACE_H
#define HIVE_ENTROPY_NODE_INTERFACE_H

#include <string>
#include <vector>

#include "Matrix.h"
#include "CoapEndpoint.h"
#include "Message.h"

using namespace std;

/**
 * @brief Class representation of an Node (Worker or Captain) in the Hive Entropy network 
 */
class HiveEntropyNodeInterface{
    public:
        
        /**
         * Constructor for the Node.
         * @param uri The uri to use as a root for this Node.
        */
        HiveEntropyNodeInterface(string uri);

        /**
         * Destructor for the Node.
         */
        ~HiveEntropyNodeInterface();

        /**
         * @brief Used to send a Message on the network. 
         *
         * Intended to be used by users that master the application-level protocol.
         * @param message The message object to be sent.
         */
        virtual void send(Message message);

        //Specialized sends
        template<typename T>
        void sendMatrixMultiplicationTask(string uri, Matrix<T> a, Matrix<T> b, int processors, int steps, string taskId, string calculationId);

        template<typename T>
        void sendMatrixConvolutionTask(string uri, Matrix<T> a, Matrix<T> b, int processors, int steps, string taskId, string calculationId, int borderSize);

        virtual bool checkLiveness(string uri);
        virtual int queryNodeAvailability();
        virtual vector<string> resolveNodes();

        //Response and new message handlers
        virtual void registerResponseHandler(string key, void  (*func)(string origin,Message message));
        virtual void registerMessageHandler(string key, Message  (*func)(string origin,Message message));

    private:
        string uri;
        vector<string,void  (*)(string origin,Message message)> responseHandlers;
        vector<string,Message  (*)(string origin,Message message)> messageHandlers;
        CoapEndpoint coap;
};

#endif