#ifndef HIVE_ENTROPY_NODE_H
#define HIVE_ENTROPY_NODE_H

#include "HiveEntropyNodeInterface.h"
#include "Row.h"
#include "Column.h"

class HiveEntropyNode : public HiveEntropyNodeInterface{
    public:
        HiveEntropyNode(std::string uri);
        ~HiveEntropyNode();

        void send(Message m);

        template<typename T>
        void sendMatrixMultiplicationTask(string target, Matrix<T> a, Matrix<T> b, int insertX, int insertY, int steps, string taskId, string calculationId);
        template<typename T>
        void sendMatrixMultiplicationTask(string target,Row<T> rows, Column<T> col, string calculationId);
        template<typename T>
        void sendMatrixConvolutionTask(string target, Matrix<T> a, Matrix<T> b, string calculationId, int borderSize);

        void checkLiveness(string target);
        void queryNodeAvailability();
        void resolveNodeIdentities();

        void registerResponseHandler(coap_response_handler_t func);
        void registerMessageHandler(string uri, HttpMethod method, coap_method_handler_t func);
    private:
        CoapEndpoint coap;
        int banana=5;
};

#endif