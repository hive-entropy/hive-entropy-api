#ifndef HIVE_ENTROPY_NODE_INTERFACE_H
#define HIVE_ENTROPY_NODE_INTERFACE_H

#include <string>
#include <vector>

#include "Matrix.h"
#include "CoapEndpoint.h"
#include "Message.h"

using namespace std;

class HiveEntropyNodeInterface{
    public:
        //Lifecycle methods
        HiveEntropyNodeInterface();
        ~HiveEntropyNodeInterface();

        //Generic send
        virtual void send(string uri, Message message);

        //Specialized sends
        template<typename T>
        void sendMatrixMultiplicationTask(string uri, Matrix<T> a, Matrix<T> b, int processors, int steps, string taskId, string calculationId);

        template<typename T>
        void sendMatrixConvolutionTask(string uri, Matrix<T> a, Matrix<T> b, int processors, int steps, string taskId, string calculationId, int borderSize);

        virtual bool checkLiveness(string uri);
        virtual int queryNodeAvailability();
        virtual vector<string> resolveNodes();

        //Response and new message handlers
        virtual void registerResponseHandler(void  (*func)(string origin,Message message));
        virtual void registerMessageHandler(Message  (*func)(string origin,Message message));

    private:
        string uri;
        vector<void  (*)(string origin,Message message)> responseHandlers;
        vector<Message  (*)(string origin,Message message)> messageHandlers;
        CoapEndpoint coap;
};

#endif